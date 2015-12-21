#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "sockopt.h"
#include "logger.h"

int socketServer(const char *bind_ipaddr, const int port, \
		const char *szLogFilePrefix)
{

	struct sockaddr_in bindaddr;
	int sock;
	int result;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d"\
			"socket creat file,errno= %d,err info= %s\n",\
			__LINE__,errno,strerror(errno));
		return -1;
	}

	result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &result, sizeof(int));
	if (result <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d"\
			"setsockopt failed,errno= %d,err info= %s\n",\
			__LINE__,errno,strerror(errno));
		close(sock);
		return -2;
	}

	bindaddr.sin_family = AF_INET;
	bindaddr.sin_port = htons(port);
	if (NULL == bind_ipaddr || '\0' == bind_ipaddr[0])
	{
		bindaddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if (0 == inet_aton(bind_ipaddr,&bindaddr.sin_addr))
		{
			logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,"\
			"invalid ip %s,errno = %d ,err info = %s\n",\
			__LINE__,bind_ipaddr,errno,strerror(errno));

			close(sock);
			return -3;
		}
	}

	result = bind(sock,(struct sockaddr*)&bindaddr, sizeof(bindaddr));
	if (result <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,"\
			"bind port fail,errno = %d,err info = %s\n",\
			__LINE__,errno,strerror(errno));

		close(sock);
		return -4;
	}

	result = listen(sock,5);
	if (result <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,"\
			"listen port fail,errno = %d,err info = %s\n",\
			__LINE__,errno,strerror(errno));

		close(sock);
		return -5;
	}

#ifdef __DEBUG__
	printf("listen...!,socketServer done! file :%s,line:%d\n",\
			__FILE__,__LINE__);
#endif

	return sock;
}

int nbaccept(int sock,int timeout,int *err_no)
{

	struct sockaddr_in inaddr;
	unsigned int sockaddr_len;
	fd_set read_set;
	fd_set exception_set;
	struct timeval t;
	int result;

	if (timeout > 0)
	{
		t.tv_sec =timeout;
		t.tv_usec = 0;

		FD_ZERO(&read_set);
		FD_ZERO(&exception_set);
		FD_SET(sock,&read_set);
		FD_SET(sock,&exception_set);

		result = select(sock+1,&read_set,NULL,&exception_set,&t);
		if (0 == result)
		{
			*err_no = ETIMEDOUT;
			return -1;
		}
		else if (result <0)
		{
			*err_no = errno;
			return -1;
		}
		if (!FD_ISSET(sock,&read_set))
		{
			*err_no = EAGAIN;
			return -1;
		}
	}
	sockaddr_len = sizeof(inaddr);
	result = accept(sock,(struct sockaddr *)&inaddr,&sockaddr_len);
	if (result <0)
	{
		*err_no = errno;
	}
	else
	{
		*err_no = 0;
	}

#ifdef __DEBUG__
	printf("nbaccept done! file:%s,line:%d\n",\
			__FILE__,__LINE__);
#endif
	return result;
}

in_addr_t getIpaddr(getnamefunc getname, int sock, char *buff, const int bufferSize)
{
	struct sockaddr_in addr;
	int addrlen;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if (getname(sock, (struct sockaddr *)&addr, &addrlen) != 0)
	{
		buff[0] = '\0';
		return INADDR_NONE;
	}

	if (addrlen > 0)
	{
		snprintf(buff, bufferSize, "%s", inet_ntoa(addr.sin_addr));
	}
	else
	{
		buff[0] = '\0';
	}

	return addr.sin_addr.s_addr;
}

int tcprecvdata(int sock,void* data,int size,int timeout)
{

	return 1;
}