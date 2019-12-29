#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

float AN_CUTOFF;

float AN_how_english_1(char* str);
float AN_how_english_2(char* str);
float AN_how_english_3(char* str);
float AN_dev(char* str);
float AN_ioc(char* str);
float AN_periodicity(char* str, int period);
float AN_quad(char *str);

#endif