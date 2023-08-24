#include <kernel/klib/string.h>

int strcmp(const char *s1, const char *s2)
{
	size_t i, s1_len, s2_len;

	i = 0;
	s1_len = strlen(s1);
	s2_len = strlen(s2);

	if (s1_len != s2_len)
		return -1;

	while (i < s1_len) {
		if (s1[i] != s2[i])
			return -1;
		i++;
	}

	return 0;
}
