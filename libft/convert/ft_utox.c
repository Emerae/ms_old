
#include "libftfull.h"
#include <stdlib.h>

static int	utox_size(unsigned int nb)
{
	if (nb < 16)
		return (1);
	return (1 + utox_size(nb / 16));
}

char		*ft_utox(unsigned int n)
{
	char	*res;
	char	*base;
	int		i;
	int		l;

	l = utox_size(n);
	base = "0123456789abcdef";
	res = malloc(sizeof(char) * (l + 1));
	if (!res)
		return (NULL);
	res[l] = '\0';
	if (n == 0)
		res[0] = '0';
	i = l - 1;
	while (i >= 0)
	{
		res[i] = base[n % 16];
		n = n / 16;
		i--;
	}
	return (res);
}
