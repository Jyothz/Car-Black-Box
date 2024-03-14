#ifndef DASHBOARD_H
#define DASHBOARD_H
/*function to dsiplay dashboard item*/
void display_dashboard(unsigned char);

/*fuction to display time in dash board*/
void display_time(void);

/*display the current event*/
void display_event(char *event);

/*display spped using POT value ADC*/
void display_speed(int);
#endif