/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
void jepath(char*);
void jesetpath(char*);
void* jeload(void* callbacks); // returns J
int jefirst(int,char*);
int jedo(char*);
void jefree();
char* jegetlocale();
void jefail(char*);
void* jega(I t, I n, I r, I*s); // returns A
void* jehjdll();
