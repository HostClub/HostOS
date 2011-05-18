
struct _shell_proc
{
	struct _shell_proc * next;
	char * program_name;
	void (* function_pointer)(char * arguements);
};

void main();

void load_function(char * program_name , void (* function_pointer)(char * arguements));

struct _shell_proc * first;
struct _shell_proc * last;
