bool UzeNetConnect(){
   WaitVsync(60);
   return true;
}

bool UzeNetGetList(){//get list of matching games
   WaitVsync(60);
   return true;
}

void UzeNetServerNames(char * addr, u8 start, u8 entries){//address to write to, number of entries each name is 16 bytes long
   for(u8 i=0;i<entries;i++)
   for(u8 j=0;j<16;j++){
      *addr = 'S';
      addr++;
   }
   

   WaitVsync(1);
}
