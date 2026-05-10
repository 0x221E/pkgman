#ifndef COOKBOOK_H
#define COOKBOOK_H

typedef struct url url;
typedef struct string_view string_view;

int cookbook_recipe_run(struct url *script_loc,
			char *script,
			struct string_view *out);

#endif
