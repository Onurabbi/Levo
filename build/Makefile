SYS_DIR = ../code/system
JSON_DIR = ../code/json
GAME_DIR = ../code/game
INCLUDES = ../code/common.h ../code/defs.h ../code/structs.h 
#LFLAGS = -L "C:/Dev/Dependencies/lib/lib"
LIBS = -lSDL2main -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2
OBJECTS = main.o init.o texture.o draw.o atlas.o animation.o utils.o input.o \
sound.o entity.o cJSON.o entityFactory.o text.o dungeon.o title.o widget.o asset.o \
collision.o actor.o player.o job.o memory.o

CC = gcc -O3 -DNDEBUG

game: $(OBJECTS) 
	$(CC) $(INCLUDE) $(LFLAGS) $(OBJECTS) -o game $(LIBS)

main.o: ../code/main.c $(INCLUDES)
	$(CC) $(LFLAGS) -c ../code/main.c $(LIBS)

#SYSTEM
init.o: $(SYS_DIR)/init.c $(SYS_DIR)/init.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/init.c 
texture.o: $(SYS_DIR)/texture.c $(SYS_DIR)/texture.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/texture.c 
draw.o: $(SYS_DIR)/draw.c $(SYS_DIR)/draw.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/draw.c 
asset.o: $(SYS_DIR)/atlas.c $(SYS_DIR)/asset.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/asset.c 
atlas.o: $(SYS_DIR)/atlas.c $(SYS_DIR)/atlas.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/atlas.c 
animation.o: $(SYS_DIR)/animation.c $(SYS_DIR)/animation.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/animation.c 
utils.o: $(SYS_DIR)/utils.c $(SYS_DIR)/utils.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/utils.c 
input.o: $(SYS_DIR)/input.c $(SYS_DIR)/input.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/input.c 
sound.o: $(SYS_DIR)/sound.c $(SYS_DIR)/sound.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/sound.c
text.o: $(SYS_DIR)/text.c $(SYS_DIR)/text.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/text.c 
widget.o: $(SYS_DIR)/widget.c $(SYS_DIR)/widget.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/widget.c 
job.o: $(SYS_DIR)/job.c $(SYS_DIR)/job.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/job.c 
memory.o: $(SYS_DIR)/memory.c $(SYS_DIR)/memory.h
	$(CC) $(LFLAGS) -c $(SYS_DIR)/memory.c 

#GAME
entity.o: $(GAME_DIR)/entity.c $(GAME_DIR)/entity.h
	$(CC) $(LFLAGS) -c $(GAME_DIR)/entity.c 
entityFactory.o : $(GAME_DIR)/entityFactory.c $(GAME_DIR)/entityFactory.h
	$(CC) $(LFLAGS) -c $(GAME_DIR)/entityFactory.c 
dungeon.o: $(GAME_DIR)/dungeon.c $(GAME_DIR)/dungeon.h
	$(CC) $(LFLAGS) -c $(GAME_DIR)/dungeon.c 
title.o: $(GAME_DIR)/title.c $(GAME_DIR)/title.h
	$(CC) $(LFLAGS) -c $(GAME_DIR)/title.c 
collision.o: $(GAME_DIR)/collision.c $(GAME_DIR)/collision.h
	$(CC) -c $(GAME_DIR)/collision.c
actor.o: $(GAME_DIR)/actor.c $(GAME_DIR)/actor.h
	$(CC) -c $(GAME_DIR)/actor.c
player.o: $(GAME_DIR)/player.c $(GAME_DIR)/player.h
	$(CC) -c $(GAME_DIR)/player.c

#MISC
cJSON.o: $(JSON_DIR)/cJSON.c $(JSON_DIR)/cJSON.h
	$(CC) $(LFLAGS) -c $(JSON_DIR)/cJSON.c

clean:
	rm *.o game