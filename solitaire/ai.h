uint8_t ai_action;
uint8_t ai_state,ai_targetx,ai_targety,ai_freshdeck,ai_step,ai_skip_tick;

uint8_t ai_last_state,ai_target_stack,ai_last_source;//debugging only
uint16_t ai_prng;

#define AI_DISABLED	0
#define AI_CHOOSING	1
#define AI_CLICK	2
#define AI_SHORTCUT	4
#define AI_DRAW		8
#define AI_WAIT		16
#define AI_HOLD		32

void AIPickMove(){

//ai_state = AI_SHORTCUT;
//ai_targetx = (stacks[STACK_BASE1].x*8)+4;
//ai_targety = (stacks[STACK_BASE1].y*8)+((stacks[STACK_BASE1].numcards-1)*8);
//return;


	//first check all decks on the table, see if there is a facedown one available. flip it if so
	for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++){
		if(stacks[i].numcards && !(stacks[i].cards[stacks[i].numcards-1] & FACEUP)){
			ai_state = AI_CLICK;
			ai_targetx = (stacks[i].x*8)+8;
			ai_targety = (stacks[i].y*8)+((stacks[i].numcards-1)*8)+8;
			ai_target_stack = i;
			return;
		}
	}



	//see if we can send some cards up to the home stacks
	for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++){
		if(!stacks[i].numcards)
			continue;
		stack_held = true;//need this for the overlap function to work right
		stacks[STACK_HELD].numcards = 1;
		stacks[STACK_HELD].cards[0] = stacks[i].cards[stacks[i].numcards-1];
		stack_held_src = i;

		for(uint8_t j=STACK_HOME0;j<=STACK_HOME3;j++){
			if(CanGoOnStack(j)){
//while(1){}
				stack_held = 0;
				stacks[STACK_HELD].numcards = 0;
				ai_state = AI_SHORTCUT;
				ai_targetx = (stacks[i].x*8)+8;
				ai_targety = (stacks[i].y*8)+((stacks[i].numcards-1)*8);
				ai_target_stack = i;
				return;
			}
		}
		stack_held = 0;
		stacks[STACK_HELD].numcards = 0;
	}


	//check if we can move a faceup stack somewhere, that has a facedown card underneath
	//this automatically avoids moving stacks back and forth in a looping manner, since the facedown card is higher priority
	stack_held = true;
	for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++){
		if(!stacks[i].numcards)
			continue;
		uint8_t j=0;
		if((stacks[i].cards[0] & FACEUP) && stacks[i].numcards > 1)//no face down card in this stack, so don't bother moving it...unless it's the only card
			continue;
		
		stack_held = false;
		for(j=0;j<stacks[i].numcards;j++){//look for the first FACEUP card, there is one if we got this far.
			if(stacks[i].cards[j] & FACEUP){
				stack_held = true;
				stack_held_src = i;
				stacks[STACK_HELD].numcards = 1;
				stacks[STACK_HELD].cards[0] = stacks[i].cards[j];
				break;
			}
		}
		if(!stack_held)
			continue;
		for(uint8_t k=STACK_BASE0;k<=STACK_BASE6;k++){
			if(k == i)//if(/*!stacks[k].numcards || */k == i || k == ai_last_source)
				continue;
			if(stacks[i].numcards == 1 && stacks[k].numcards == 0){//don't get stuck in a loop
				continue;
			}
			if(CanGoOnStack(k)){
				stack_held = 0;
				stacks[STACK_HELD].numcards = 0;
				ai_state = AI_SHORTCUT;
				ai_targetx = (stacks[i].x*8)+8;
				ai_targety = (stacks[i].y*8)+((j)*8);
				ai_target_stack = i;
//while(1){SPrintNum(20,20,i,1);SPrintNum(20,20,k,1);}
				return;
			}
		}
	}



	//check if we can send a card right from the draw pile to the home stacks
	if(stacks[STACK_DRAW].numcards){
	
	stack_held = true;//need this for the overlap function to work right
	stacks[STACK_HELD].numcards = 1;
	stacks[STACK_HELD].cards[0] = stacks[STACK_DRAW].cards[stacks[STACK_DRAW].numcards-1];
	for(uint8_t i=STACK_HOME0;i<=STACK_HOME3;i++){
		stacks[STACK_HELD].cards[0] = stacks[STACK_DRAW].cards[stacks[STACK_DRAW].numcards-1];
		if(CanGoOnStack(i)){
			stack_held = 0;
			stacks[STACK_HELD].numcards = 0;
			ai_state = AI_SHORTCUT;
			ai_targetx = (stacks[STACK_DRAW].x*8)+(0)+8;//draw 3 is horizontal...TODO
			ai_targety = (stacks[STACK_DRAW].y*8)+16;
			return;
		}
	}
	stack_held = 0;
	stacks[STACK_HELD].numcards = 0;
	}


	//see if we can send a card from the draw stack to a base pile
	if(stacks[STACK_DRAW].numcards){
	stack_held = true;
	stack_held_src = STACK_DRAW;
	stacks[STACK_HELD].cards[0] = stacks[STACK_DRAW].cards[stacks[STACK_DRAW].numcards-1];
	for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++){
		if(CanGoOnStack(i)){
			stack_held = false;
			stacks[STACK_HELD].numcards = 0;
			ai_state = AI_SHORTCUT;
			ai_targetx = (stacks[STACK_DRAW].x*8)+(0)+8;//draw 3 is horizontal...TODO
			ai_targety = (stacks[STACK_DRAW].y*8)+16;
			return;
		}
	}
	stack_held = false;
	stacks[STACK_HELD].numcards = 0;
	}

	//if nothing else, draw a card. if there is no card left on the deck, see if we have entirely ran through the deck trying
	//if so, then there is nothing else to do, since we would have tried every possibility before resorting to draws
	//the AI is not smart enough to take stuff back off the home piles, but this simple strategy works a good amount of the time(for single draw)
	if(!stacks[STACK_DECK].numcards){//set the flag, so we know we have attempted this through the entire deck without finding a move
		if(!ai_freshdeck)
			ai_freshdeck = 1;
		else{//we can't find any moves, give up
			ai_state = 0;
			ai_freshdeck = 0;
			return;
		}
	}
	ai_state = AI_DRAW;
	ai_targetx = (stacks[STACK_DRAW].x*8)+(0)+8;//draw 3 is horizontal...TODO
	ai_targety = (stacks[STACK_DRAW].y*8)+16;

}

uint8_t AIMoveToCard(uint8_t holdcard){
		uint8_t ret = 0;
		if(ai_targetx > cursorx){
			padstate |= BTN_RIGHT;
			ret = 1;;
		}else if(ai_targetx+4 < cursorx){
			padstate |= BTN_LEFT;
			ret = 1;
		}

		if(ai_targety > cursory){
			padstate |= BTN_DOWN;
			ret = 1;
		}else if(ai_targety+4 < cursory){
			padstate |= BTN_UP;
			ret = 1;
		}
		return ret;
}

void AIUpdate(){
	if(game_state & CARD_FLOATING || !(game_state & CARDS_DEALT)){
		padstate = 0;
		return;
	}
	if(ai_skip_tick){
		ai_skip_tick = 0;
		return;
	}

/////////////////SOMETIMES AI WILL NOT STACK A DECK(IF IT HAS NO FACEDOWN CARDS?) ONTO ANOTHER EVEN WHEN IT COULD(AND GIVE UP BEFORE DOING SO)
/////////////////SHOULD ALWAYS DO THIS IN CASE WE ENCOUNTER A KING...
//SPrintNum(0,VRAM_TILES_V-1,1,0);
	
	//THE AI CAN NEVER WIN, THIS IS HANDLED IN CHECKVICTORY() SINCE THEY COULDN'T ENTER A HIGH SCORE...

//TriggerFx(1,255,true);
//SPrintNum(20,0,ai_state,1);
//SPrintNum(20,1,ai_targetx,1);
//SPrintNum(20,2,ai_targety,1);
//SPrintNum(20,4,ai_last_state,1);
//SPrintNum(20,5,ai_target_stack,1);
	if(ai_state == AI_DISABLED)
		return;
//	uint8_t tx,ty;
	if(ai_state == AI_HOLD){
			padstate |= BTN_A;
			if(ai_targetx){
				ai_targetx--;
				return;
			}
			//ai_state = AI_CHOOSING;//find the next thing to do
			ai_state = AI_WAIT;
			ai_targetx = (GetPrngNumber(0)%30)+15;
			return;
	}else
		padstate = 0;

//if(ai_state == AI_WAIT)
//	ai_state = AI_CHOOSING;


	if(ai_state == AI_WAIT){
		if(ai_targetx)
			ai_targetx--;
		else
			ai_state = AI_CHOOSING;

	}else if(ai_state == AI_CHOOSING){

		AIPickMove();
		if(ai_state == AI_CLICK || ai_state == AI_SHORTCUT){
			if(stacks[ai_target_stack].numcards == 1)//KLUDGE for different alignment between single card stacks and multi card stacks
				ai_targety += 8;
		}


	}else if(ai_state == AI_CLICK){//if we are not at the source card, move to it. else flip it
		ai_freshdeck = 0;
padstate = BTN_SR;
		if(!AIMoveToCard(0)){//got to target
			padstate = BTN_B;//flip it
			//ai_state = AI_CHOOSING;//find the next thing to do
			ai_last_state = ai_state;
			ai_state = AI_WAIT;
			ai_targetx = (GetPrngNumber(0)%30)+15;
		}
	}else if(ai_state == AI_SHORTCUT){//send a card somewhere
		ai_freshdeck = 0;
padstate = BTN_SR;
		if(!AIMoveToCard(0)){
			padstate = (BTN_B);//shortcut it to the base stack
			//ai_state = AI_CHOOSING;//find the next thing to do
			ai_last_source = ai_target_stack;
				ai_last_state = ai_state;
			ai_state = AI_HOLD;//have to hold B for 1 frame before pushing A for shortcut to work...
			ai_targetx = 8;
		}
	}else if(ai_state == AI_DRAW){//draw a card/cards
//TriggerFx(1,255,true);
padstate = BTN_SR;
			if(!AIMoveToCard(0)){
				padstate = BTN_Y;
				//ai_state = AI_CHOOSING;//find the next thing to do
				ai_last_state = ai_state;
				ai_state = AI_WAIT;
				ai_targetx = (GetPrngNumber(0)%30)+15;
			}
	}
}
