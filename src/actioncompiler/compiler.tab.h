typedef union {
  Buffer action;
  char *str;
  SWFActionFunction function;
  SWFGetUrl2Method getURLMethod;
} YYSTYPE;
#define	BREAK	257
#define	FOR	258
#define	CONTINUE	259
#define	IF	260
#define	ELSE	261
#define	DO	262
#define	WHILE	263
#define	THIS	264
#define	EVAL	265
#define	TIME	266
#define	RANDOM	267
#define	LENGTH	268
#define	INT	269
#define	CONCAT	270
#define	DUPLICATECLIP	271
#define	REMOVECLIP	272
#define	TRACE	273
#define	STARTDRAG	274
#define	STOPDRAG	275
#define	ORD	276
#define	CHR	277
#define	CALLFRAME	278
#define	GETURL	279
#define	LOADMOVIE	280
#define	LOADVARIABLES	281
#define	POSTURL	282
#define	SUBSTR	283
#define	NEXTFRAME	284
#define	PREVFRAME	285
#define	PLAY	286
#define	STOP	287
#define	TOGGLEQUALITY	288
#define	STOPSOUNDS	289
#define	GOTOFRAME	290
#define	FRAMELOADED	291
#define	SETTARGET	292
#define	STRING	293
#define	NUMBER	294
#define	IDENTIFIER	295
#define	GETURL_METHOD	296
#define	EQ	297
#define	LE	298
#define	GE	299
#define	NE	300
#define	LAN	301
#define	LOR	302
#define	INC	303
#define	DEC	304
#define	IEQ	305
#define	DEQ	306
#define	MEQ	307
#define	SEQ	308
#define	STREQ	309
#define	STRNE	310
#define	STRCMP	311
#define	PARENT	312
#define	END	313
#define	UMINUS	314
#define	POSTFIX	315
#define	NEGATE	316


extern YYSTYPE yylval;
