/* UNIX v7 usr/src/games/hangman.c
 *
 * Copyright 2002 Caldera International Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code and documentation must retain the
 * above copyright notice, this list of conditions and the following
 * disclaimer. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *
 * This product includes software developed or owned by Caldera
 * International, Inc.
 *
 * Neither the name of Caldera International, Inc. nor the names of
 * other contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENCE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/time/time.h"
#include "third_party/zlib/zlib.h"
// clang-format off

#define DICT "usr/share/dict/hangman"
#define MAXERR 7
#define MINSCORE 0
#define MINLEN 7

char *dictfile;
int alive,lost;
FILE *dict;
long int dictlen;
float errors=0, words=0;
char word[26],alph[26],realword[26];

void fatal(s)
	char *s;
{
	fprintf(stderr,"%s\n",s);
	exit(1);
}

void setup()
{
	long tvec;
	struct stat statb;
	time(&tvec);
	srand(tvec);
	if((dict=fopen(dictfile,"r"))==NULL) fatal("no dictionary");
	if(stat(dictfile,&statb)<0) fatal("can't stat");
	dictlen=statb.st_size;
}

double frand()
{
	return(rand()/32768.);
}

void pscore()
{
	if(words!=0) printf("(%4.2f/%.0f) ",errors/words,words);
}

void getword()
{	char wbuf[128],c;
	int i,j;
loop:
	if(fscanf(dict,"%s\n",wbuf)==EOF)
	{	fseek(dict,0L,0);
		goto loop;
	}
	if((c=wbuf[0])>'z' || c<'a') goto loop;
	for(i=j=0;wbuf[j]!=0;i++,j++)
	{	if(wbuf[j]=='-') j++;
		wbuf[i]=wbuf[j];
	}
	wbuf[i]=0;
	if(i<MINLEN) goto loop;
	for(j=0;j<i;j++)
		if((c=wbuf[j])<'a' || c>'z') goto loop;
	pscore();
	strcpy(realword,wbuf);
	for(j=0;j<i;word[j++]='.');
}

void startnew()
{	int i;
	long int pos;
	char buf[128];
	for(i=0;i<26;i++) word[i]=alph[i]=realword[i]=0;
	pos=frand()*dictlen;
        pos%=dictlen;
	fseek(dict,pos,0);
	fscanf(dict,"%s\n",buf);
	getword();
	alive=MAXERR;
	lost=0;
}

void stateout()
{	int i;
	printf("guesses: ");
	for(i=0;i<26;i++)
		if(alph[i]!=0) putchar(alph[i]);
	printf(" word: %s ",word);
	printf("errors: %d/%d\n",MAXERR-alive,MAXERR);
}

void getguess()
{	char gbuf[128],c;
	int ok=0,i;
loop:
	printf("guess: ");
	if(gets(gbuf)==NULL)
	{	putchar('\n');
		exit(0);
	}
	if((c=gbuf[0])<'a' || c>'z')
	{	printf("lower case\n");
		goto loop;
	}
	if(alph[c-'a']!=0)
	{	printf("you guessed that\n");
		goto loop;
	}
	else alph[c-'a']=c;
	for(i=0;realword[i]!=0;i++)
		if(realword[i]==c)
		{	word[i]=c;
			ok=1;
		}
	if(ok==0)
	{	alive--;
		errors=errors+1;
		if(alive<=0) lost=1;
		return;
	}
	for(i=0;word[i]!=0;i++)
		if(word[i]=='.') return;
	alive=0;
	lost=0;
	return;
}

void wordout()
{
	errors=errors+2;
	printf("the answer was %s, you blew it\n",realword);
}

void youwon()
{
	printf("you win, the word is %s\n",realword);
}

int main(int argc,char **argv)
{
	if(argc==1) dictfile=DICT;
	else dictfile=argv[1];
	setup();
	for(;;)
	{	startnew();
		while(alive>0)
		{	stateout();
			getguess();
		}
		words=words+1;
		if(lost) wordout();
		else youwon();
	}
}
