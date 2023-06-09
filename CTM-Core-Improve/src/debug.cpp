#include "debug.h"
#include <time.h>

//extern int present_clock;

debug::debug():err(NULL){
	char *str = filename;
	time_t run;
	time(&run);
	struct tm *tp = localtime(&run);
	sprintf( str,"%02ld-%02ld-%04ld.log",tp->tm_mon+1,tp->tm_mday,tp->tm_year+1900 );
	err = fopen( str,"w" );
	fprintf( err,"Green Signal Log File.\n\n" );
	fputs("----------------------------------------\n",err);
	fprintf( err,"The Date: " );
	fprintf( err,"\t%s\n",ctime(&run) );

	warn_times = 0;
}

debug::debug(char *name){
	char *tmpstr = filename;
	char str[256];
	sprintf(tmpstr, "%s.log", name);
	sprintf( str,".\\Output\\%s",tmpstr );
	err = fopen( str,"w" );
	fputs("Green Signal Log File.\n",err);
	fputs("----------------------------------------\n",err);
	fprintf( err,"Input File: \t%s\n",name );
	time_t run;
	time(&run);
	localtime(&run);
	fprintf( err,"The Date: " );
	fprintf( err,"\t%s\n",ctime(&run) );

	warn_times = 0;
}

debug::~debug(){
	sprintf(tmpstr,"Total %d warnings.\n",warn_times);
	fputs(tmpstr,err);
	fputs("Green Signal Version 0.1 beta\n",err);
	fclose(err);
}

char *debug::get_str(){ return tmpstr; }

void debug::process(char *str, int present_clock){
	fputs("[Process]",err);
	fprintf( err," @%04d> %s\n\n", present_clock, str );
}

void debug::warning( char *str, int present_clock){
	fputs("[Warning]",err);
	fprintf( err," @%04d> %s\n\n", present_clock, str );
	warn_times++;
}

void debug::throws( char *str, int present_clock){
	fputs("[Error]",err);
	fprintf( err," @%04d> %s\n\n", present_clock, str );
	printf("\nFatal ERROR, see log file.\n\n");
	system("pause");
	exit(1);
}
