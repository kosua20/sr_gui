#ifndef SR_GUI_INTERNAL_H
#define SR_GUI_INTERNAL_H

#ifndef SR_GUI_MALLOC
#define SR_GUI_MALLOC(S) (malloc(S))
#endif

#ifndef SR_GUI_FREE
#define SR_GUI_FREE(S) (free(S))
#endif

#ifndef SR_GUI_MAX_STR_SIZE
#define SR_GUI_MAX_STR_SIZE 1024
#endif

#endif
