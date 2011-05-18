#include "cakesh.h"
#include "c_io.h"
#include "ulib.h"
#include "headers.h"

void test_cakesh(char * input)
{
	c_puts("In cakesh test\n");

	c_puts(input);
}

void cakesh(void)
{
	load_process("test" , test_cakesh);

	char input_buffer[4096];

	while( 1 )
	{
		c_printf("\n> ");

		char * current_buffer = input_buffer;

		char in;
		while( (in = c_getchar()) != '\n' )
		{
			//c_printf("%c" , in);
			*current_buffer = in;
			current_buffer++;
		}

		//c_printf("\n");
		
		*current_buffer = '\0';

		char * command = input_buffer;
		char * args = split(input_buffer , ' ');

		struct _shell_proc * find_program = first;

		int found_program = 0;

		while(find_program != NULL && !found_program)
		{
			if(strcmp(find_program->program_name , command))
			{

				(* find_program->function_pointer)(args);
					
				found_program = 1;
			}

			
			find_program = find_program->next;
		}

		if(!found_program)
		{
			c_puts(command);
			c_puts(": command not found");
		}

		memset(input_buffer , 0 , 4096);
	}
}

void load_process(char * program_name , void (* function_pointer)(char * args))
{
	struct _shell_proc * next_program = (struct _shell_proc *)kalloc(sizeof(struct _shell_proc) , 0 , 0);
	next_program->program_name = program_name;
	next_program->function_pointer = function_pointer;
	next_program->next = NULL;

	if(first == NULL)
	{
		first = next_program;
	}
	else
	{
		last->next = next_program;
	}
	
	last = next_program;
}
