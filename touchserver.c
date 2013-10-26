#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>    

#define BUFSIZE 256   

int main(int argc, char *argv[]) {  
    int client_sockfd;  
    int len;  
    struct sockaddr_in remote_addr; //服务器端网络地址结构体  
    char buf[BUFSIZ];  //数据传送的缓冲区  
    memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零  
    remote_addr.sin_family = AF_INET; //设置为IP通信  
    remote_addr.sin_addr.s_addr = inet_addr("115.156.150.44");//服务器IP地址  
    remote_addr.sin_port = htons(27015); //服务器端口号  
      
    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/  
    if((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {  
        printf("socket error\n");  
        return 1;  
    }  
      
    /*将套接字绑定到服务器的网络地址上*/  
    if(connect(client_sockfd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) < 0) {  
        printf("connect error\n");
        return 1;  
    }
  
    printf("now connected to server\n");  
      
    /*循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数*/  
    while(1) {  
        printf("Enter string to send (with 'q' to quit connetion):\n");  
        scanf("%s", buf);  
        if(!strcmp(buf, "q")) {  
            break;  
	}
        len = send(client_sockfd, buf, strlen(buf), 0);  
        len = recv(client_sockfd, buf, BUFSIZ, 0);  
        buf[len] = '\0';  
        printf("received information:\n%s\n", buf);  
    }  
    close(client_sockfd);//关闭套接字  
    return 0;  
}  
