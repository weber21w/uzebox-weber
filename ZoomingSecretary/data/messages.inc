//[]:/*!
const uint8_t bonusMessages[] PROGMEM =
"       SORRY NO BONUS!        "
"  WELL DONE YOU GET A BONUS!  ";


#define MESSAGES_SPACE		20

const uint8_t spaceMessages[] PROGMEM =
"  PLUTO IS SMALLER THAN MOON  "
"VENUS BRIGHTEST PLANET OF SKY "
"JOVIAN ARE JUPITER SATELLITES "
"VIKINGS 1 AND 2 LANDED ON MARS"
" THE HOTTEST PLANET IS VENUS  "
"  CERES IS LARGEST ASTEROID   "
" AL*SUFI SAW ANDROMEDA GALAXY "
"   TITAN HAS METHANE LAKES    "
"EUROPA HAS OXYGENE ATMOSPHERE "
"KUPIER BELT IS BEYOND NEPTUNE "
"  PIONEER 10 FLEW BY JUPITER  "
"VENERA 4 LANDED ON VENUS IN 67"
"HALLEY HAS 75*76 YEARS PERIOD "
"ALBEDO* REFLECTION COEFFICIENT"
"ECCENTRIC ORBIT * NON*CIRCULAR"
"KEPLER * LAWS OF PLANET MOTION"
" PROMETHEUS IS VOLCANO ON IO  "
"ARISTARCHUS WAS HELIOCENTRIST "
"TYCHO BRAHE MADE MEASUREMENTS "
" URANUS ROTATES ON ITS SIDE   ";


#define MESSAGES_HISTORY	9

const uint8_t historyMessages[] PROGMEM =
"MACBETH WAS A KING OF SCOTLAND"
" 3 KINGDOMS * EGYPTIAN GLORY  "
"JERICHO * EARLIEST KNOWN CITY "
" LOUIS XIV RULED FOR 72 YEARS "
"ANNO DOMINI * YEAR OF OUR LORD"
"   CE MEANS COMMON ERA * AD   "
"STONEHENGE IS OLDER THAN 4200 "
" VICTORIA RULED 1837 TO 1901  "
"SNOW IN SAHARA ON 18 FEB 1979 ";

#define MESSAGES_EARTH		9

const uint8_t earthMessages[] PROGMEM =
" MARCO POLO TRAVELLED TO ASIA "
"MAGELLAN CROSSED PACIFIC OCEAN"
" MOUNT EVEREST IS 8850 M HIGH "
" AMUNDSEN REACHED SOUTH POLE  "
"DR LIVINGSTONE EXPLORED AFRICA"
"VASCO DA GAMA SAILED TO INDIA "
"EARTH RADIUS IS ABOUT 6370 KM "
" MARIANA TRENCH IS 11 KM DEEP "
"WATER * 71 PERCENT OF SURFACE ";

#define MESSAGES_BOOKS		10

const uint8_t booksMessages[] PROGMEM =
"RHYMES ARE EASIER TO REMEMBER "
"VIRGIL IS THE AUTHOR OF AENEID"
"   OVID WROTE METAMORPHOSES   "
"EURIPIDES WON PRIZE FOR MEDEA "
"AESCHYLUS INTRODUCED DIALOGUE "
"   TAO TE CHING IS BY LAOZI   "
"DIVINE COMEDY WRITTEN BY DANTE"
" PUSHKIN WROTE EUGENE ONEGIN  "
" ONEGIN IS ABOUT TRENDY DANDY "
"  COLERIDGE WROTE KUBLA KHAN  ";

const uint8_t* const topicMessages[4] PROGMEM = {
earthMessages,
historyMessages,
booksMessages,
spaceMessages
};

const uint8_t topicMessagesCount[4] PROGMEM = {
MESSAGES_EARTH,
MESSAGES_HISTORY,
MESSAGES_BOOKS,
MESSAGES_SPACE
};
