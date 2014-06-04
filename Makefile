lex.yy.c: json.l
	flex json.l
json.tab.c: json.y
	bison --defines json.y
mlab-ns: mlab-ns.c json.tab.c lex.yy.c json_object.c json_object.h
	gcc -o mlab-ns mlab-ns.c json.tab.c lex.yy.c json_object.c
clean:
	rm -f mlab-ns a.out core json.tab.c json.tab.h lex.yy.c 

#a.out   json.tab.c  json.y    Makefile  mlab-ns.c
#json.l  json.tab.h  lex.yy.c  mlab-ns
