#include<reg51.h>

#define GPIO_DIG P0
#define GPIO_KEY P1

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
//sbit dbg=P2^0;

unsigned char code DIG_CODE[11]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};

unsigned char key,num,lock,oper,t;
long buf,n1,n2;
unsigned char d[8];

void display();
void render(long n);
unsigned char digit(long n,unsigned char m);
unsigned char waitkey();
void delay();
void pressnum(unsigned char n);
void docalc();

void main(){
	buf=0;
	lock=0;
	oper=0;
	while(1){
		render(buf);
		while(!waitkey())delay();
		if((key<11&&key%4<3)||key==12){
			switch(key){
				case(0):num=7;break;
				case(1):num=8;break;
				case(2):num=9;break;
				case(4):num=4;break;
				case(5):num=5;break;
				case(6):num=6;break;
				case(8):num=1;break;
				case(9):num=2;break;
				case(10):num=3;break;
				case(12):num=0;break;
			}
			pressnum(num);	
		}else if(buf>=0){
			t=5;
			switch(key){
				case(13)://backspace
					if(!lock)buf/=10;
					break;
				case(14)://=
					docalc();
					break;
				case(15)://+
					t--;
				case(11)://-
					t--;
				case(7)://*
					t--;
				case(3):///
					t--;
					docalc();
					n1=buf;
					oper=t;
			}
		}
	}	
}

void docalc(){
	n2=buf;
	lock=1;
	switch(oper){
		case(1)://+
			buf=n1+n2;
			break;
		case(2)://-
			buf=n1-n2;
			break;
		case(3)://*
			buf=n1*n2;
			break;
		case(4):///
			if(n2)
				buf=n1/n2;
			else
				buf=-1;
			break;
	}
	oper=0;
	if(buf<0||buf>99999999)buf=-1;
}

void pressnum(unsigned char n){
	if(lock)
		buf=n,lock=0;
	else
		if(buf<=9999999)buf=buf*10+n;
}

unsigned char waitkey(){
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay();
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{
			
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	key=0;break;
				case(0X0b):	key=1;break;
				case(0X0d): key=2;break;
				case(0X0e):	key=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	break;
				case(0Xb0):	key+=4;break;
				case(0Xd0): key+=8;break;
				case(0Xe0):	key+=12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				delay();
				a++;
			}
			return 1;
		}
	}
	return 0;
}

void render(long n){
	unsigned char i;
	unsigned char z=1;
	unsigned char c;
	if(buf>=0){
		for(i=7;i<=7;i--){
			c=digit(n,i);
			if(c==0&&z&&i>0){
				d[i]=0;
			}else{
				d[i]=DIG_CODE[c];
				z=0;
			}
		}
	}else{
		d[7]=0x79;
		d[6]=d[5]=d[3]=0x50;
		d[4]=0x5c;
		d[2]=d[1]=d[0]=0;
	}				
}

unsigned char digit(long n,unsigned char m){
	int i;
	for(i=0;i<m;i++)n/=10;
	return n%10; 	
}

void display(){
	unsigned char i;
	unsigned char j;
	for(i=0;i<8;i++)
	{
		LSA=i&1;
		LSB=(i&2)/2;
		LSC=(i&4)/4;
		GPIO_DIG=d[i];//发送段码
		j=10;						 //扫描间隔时间设定
		while(j--);	
		GPIO_DIG=0x00;//消隐
	}
}

void delay(){
	unsigned char i=10;
	while(i--)display();
}