#include "updateprompt.h"
#include <string.h>

void
update_prompt(char prompt[PRMTLEN], char *new_prompt_text)
{
	snprintf(prompt, PRMTLEN - 1, "(%s)", new_prompt_text);
}
