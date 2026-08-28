NAME = ft_ls

CC = cc

CFLAGS = -Wall -Werror -Wextra 

SRCS =  src/main.c\
	src/ft_ls.c\
	src/display.c\
	src/utils.c\
	src/parser.c

OBJS = $(SRCS:.c=.o)

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@

all: ${NAME}

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ${NAME}

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf ${NAME}

re: fclean all