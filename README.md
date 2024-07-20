# A smart digital clock for the modern age

Are you tired of boring, simple clocks that just work? I am!
With this clock, you can display not only the time but also whatever unnecessary information that your heart desires!
I love it!!!

Just kidding, this sucks.

Features:
- Automatic time synchronisation with an NTP server, preventing the need for manually setting the time in case the RTC clock loses power.
- An RTC Module, ensuring that the clock stays functional even when not connected to the internet.
- A simple, modular menu system!

Adding screens to the menu is as easy as: 
- Subclassing the ```Screen``` class,
- Overriding the ```init()``` and ```draw()``` methods,
- Adding a pointer to an instance of your class to the ```modes``` vector in ```Digital_Clock.cpp``` ! 


