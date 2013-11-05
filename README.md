Metro-plan driving software
---------------------------

This software, written in *Arduino* langage (C derivative) drives the **partial Paris' Métro-plan** I created using electronics and LEDs to show stations.<br/>

  - Takes input from the user (buttons, LCD screen)
  - Calculates the "best" itinerary (looking for least changes then the least number of stations you have to go through)


Possible improvements:

  - Turn it into a fully-fledged algorithm that calculates every possible path at the same time with weighs (e.g one station: 1, one change: 3) → must manage carefully the Arduino's memory pool in order to achieve this, if it is at all possible
  - Expand it physically and in the software to the whole Métro stations (currently 86, total 301)
