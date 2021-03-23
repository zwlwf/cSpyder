#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void error() {
	printf("not match\n");
	exit(0); // 这个exit 可以跳出调用他的python程序
}

int isMale( char*s) {
	return strstr(s, "男")!=NULL;
}

//fprintf(fp,"name,ID,report_id,sex,age\n");
void dealingWithResponse( void* buffer) {
	FILE *fp = fopen("out2.csv", "a+"); // 若用freopen("out.txt", "a+", stdout); 则调用他的程序以及之后的程序均输出到文件中，包括调用他的python库
	int i,j;
	char *p = buffer;
	char ans[1000]={0};
	p = strstr(p, "<tr"); 
	if( !p) { error() ; };
	p+=3;
	p = strstr(p, "<td"); // nameLabel
	if( !p) { error() ; };
	p+=3;
	p = strstr(p, "<td"); 
	if( !p) { error() ; };
	p+=3;
	while( *p!='>') p++; p++; 
	i = 0;
	while(p[i]!='<') i++;
	strncpy(ans, p, i);ans[i] = 0;
	fprintf(fp,"%s,", ans); // name
	p+=i;

	p = strstr(p, "<td"); p+=3;
	p = strstr(p, "<td"); p+=3;
	// -------------- //
	while( *p!='>') p++; p++; 
	i = 0;
	while(p[i]!='<') i++;
	strncpy(ans, p, i); ans[i] = 0;
	fprintf(fp,"'%s,", ans); // ID
	p+=i;

	p = strstr(p, "<td"); p+=3;
	p = strstr(p, "<td"); p+=3;
	// -------------- //
	while( *p!='>') p++; p++; 
	i = 0;
	while(p[i]!='<') i++;
	strncpy(ans, p, i); ans[i] = 0;
	fprintf(fp,"%s,", ans); //report ID

	p = strstr(p, "<tr");
	p+=3;

	p = strstr(p, "<td"); p+=3;
	p = strstr(p, "<td"); p+=3; 
	// -------------- //
	while( *p!='>') p++; p++; 
	i = 0;
	while(p[i]!='<') i++;
	strncpy(ans, p, i); ans[i] =0;
	fprintf(fp,"%s,", isMale(ans)? "M" : "F");// sex

	p = strstr(p, "<tr");
	p+=3;

	p = strstr(p, "<td"); p+=3;
	p = strstr(p, "<td"); p+=3; 
	// -------------- //
	while( *p!='>') p++; p++; 
	i = 0;
	while(p[i]!='<') i++;
	strncpy(ans, p, i); ans[i] = 0;
	int age=0;
	sscanf(ans,"%d", &age);
	fprintf(fp,"%d,", age);// age
	p+=i;

	const char *pat = "接收时间：";
	p = strstr(p, pat );
	if(!p) { error(); }
	p+=strlen(pat);
	i = 0;
	while(p[i]!='<') i++;
	strncpy(ans, p, i); ans[i] = 0;
	fprintf(fp,"%s\n", ans);// time of receive
	fclose(fp);
}
