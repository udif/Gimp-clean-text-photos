#if 0
#!/bin/sh
src = $0
obj = ${ src%.* }
gcc - o $obj $src
exit
#endif

int main(void)
{
	printf("Hello\n");
	return 1;
}
