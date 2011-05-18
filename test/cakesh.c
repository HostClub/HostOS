#include "cakesh.h"
#include "c_io.h"
#include "ulib.h"
#include "headers.h"

void main(void)
{
	char input_buffer[4096];

	while( 1 )
	{
		c_printf(" > ");

		char * current_buffer = input_buffer;

		char in;
		while( (in = c_readc()) != '\n' )
		{
			c_printf("%c" , in);
			*(++current_buffer) = in;
		}

		c_printf("\n");
		
		*current_buffer = '\0';

		char * command = input_buffer;
		char * args = split(input_buffer , ' ');

		struct _shell_proc * find_program = first;

		int found_program = 0;

		while(find_program != NULL && find_program->next != NULL)
		{
			if(strcmp(find_program->program_name , command))
			{
				int pid = fork();
				
				info_t info;

				info.pid = pid;

				if(pid == 0)
				{
					wait(&info);
					found_program = 1;	
				}
				else
				{
					(* find_program->function_pointer)(args);
				}
			}
		}

		if(!find_program)
		{
			c_printf("%s: command not found\n" , command);
		}

		memset(input_buffer , 0 , 4096);
	}
}

void load_process(char * program_name , void (* function_pointer)(char * args))
{
	struct _shell_proc * next_program = _kalloc(sizeof(struct _shell_proc));
	next_program->program_name = program_name;
	next_program->function_pointer = function_pointer;
	next_program->next = NULL;
	
	last->next = next_program;
	last = next_program;
}
