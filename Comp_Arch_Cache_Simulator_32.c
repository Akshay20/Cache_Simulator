/**************************************************************************************************************************************************
												COMPUTER ARCHITECTURE PROJECT 2

Design and implement a parametric cache simulator and use it to design data caches suited to solve systems of linear equations. 
The simulator should model a memory hierarchy with data and victim cache. 
Assume the LRU replacement policy and (i) write back with write allocate, and (ii) write through with no-write allocate policies.
**************************************************************************************************************************************************/

/*This code is designed on systems issuing 32-bit addresses*/


#include<stdio.h>
#include<math.h>
#include<string.h>

typedef struct block_info 		//structure to implement a block
{
	int valid; 					//to check if the block is vacant or not
	int dirty;					//to check if the block is dirty (written to cache only) or not
	int count;					//to implement LRU policy (for each access count is set to 0 and for all other blocks count is incremented by 1; The block having maximum count will be least recently used and thus, replaced.
	char block_tag[33];			//to store tag (part of the address of variable-coefficients of linear equations)
}block_info;

typedef struct set 				//structure to implement a set 
{
	block_info* set_block_info; //to point to the first block in a set
	struct set* set_next;	 	//to point to the next block in a set
}set;

typedef struct cache_stuct		//structure to implement a cache
{
	int size;					//cache size	
	int read_hits;				//to count number of read hits for a cache
	int read_miss;				//to count number of read misses for a cache
	int write_hits;				//to count number of write hits for a cache
	int write_miss;				//to count number of write misses for a cache
	int no_of_blocks;			//number of blocks in a cache (cache size/block size)
	int block_size;				//block size
	int no_of_sets;				//number of sets (number of blocks/associativity)
	int tag_bits;				//number of tag bits to identify a block uniquickly
	int group_bits;				//number of group bits to identify a set uniquickly
	set* set;					//to point to the first set of the cache
}cache_stuct;

cache_stuct* data_cache;		//data cache
cache_stuct* victim_cache;		//victim cache

//Global Variables 
int offset_bits, set_way, associativity,block,write,b,rw;
char bin[33],offset[33],tag[33],group[33]; 
int data_size,victim_size =0;

//Functions 
void decimal_binary(unsigned int n, int sel);									//To convert decimal number to a binary string of bits
int binary_decimal(char* binary);												//To convert binary string of bits to a decimal number
void address_split(int select);													//To split the 32-bit address of a data into tag and group bits
cache_stuct* cache_initialize(int cache_size);									//To initialize caches
void transfer_to_cache( unsigned int i, int rw);								//To check for hit/miss in a cache and replace if cache full
int finddet(int a1,int a2,int a3,int b1,int b2,int b3,int c1, int c2,int c3);	//To calculate determinants to solve linear eqautions using Cramer's rule

int main()
{
	int a1=3,a2=2,a3=0,b1=1,b2=-1,b3=5,c1=2,c2=1,c3=5,det,detx,dety,detz;
	int d1=-1,d2=-5,d3=-5;
	
	printf("Computer Architecture Project 2\n\n");
	printf("Please enter Data Cache size in bytes (minimum 64 bytes)\n");
	scanf("%d",&data_size);
	do
	{
	printf("\nPlease enter Associativity\n");
	printf("\t\t1.Direct Mapped\n");
	printf("\t\t2.Fully Associative\n");
	printf("\t\t3.Set Associative\n");
	scanf("%d",&associativity);
	}while(associativity!=1 && associativity!=2 && associativity!=3);
	if (associativity == 3)
			{
				printf("Please enter n-way set associativity\n");
				scanf("%d",&set_way);
			}
	printf("Please enter Block size in bytes\n");
	scanf("%d",&block);
	do
	{
	printf("Write Policy\n\n");
	printf("\t\t1.Write back with write allocate\n");
	printf("\t\t2.Write through with no-write allocate\n");
	scanf("%d",&write);
	}while(write!=1 && write!=2);
	
	printf("Please enter Victim cache size (minimum 64 bytes)\n");
	scanf("%d",&victim_size);	
	
	b = block/sizeof(int);	
	offset_bits = log(b)/log(2);
	
	if(data_size>0)
	data_cache = cache_initialize(data_size);  		//initialize data cache
	if(victim_size>0)
	victim_cache = cache_initialize(victim_size);	//initialize victim cache

	transfer_to_cache((unsigned int)&a1,0);
	transfer_to_cache((unsigned int)&a2,0);
	transfer_to_cache((unsigned int)&a3,0);
	transfer_to_cache((unsigned int)&b1,0);
	transfer_to_cache((unsigned int)&b2,0);
	transfer_to_cache((unsigned int)&b3,0);
	transfer_to_cache((unsigned int)&c1,0);
	transfer_to_cache((unsigned int)&c2,0);
	transfer_to_cache((unsigned int)&c3,0);
	
	det=finddet(a1,a2,a3,b1,b2,b3,c1,c2,c3);
	transfer_to_cache((unsigned int)&det,1);
	
	transfer_to_cache((unsigned int)&d1,0);
	transfer_to_cache((unsigned int)&d2,0);
	transfer_to_cache((unsigned int)&d3,0);
	transfer_to_cache((unsigned int)&b1,0);
	transfer_to_cache((unsigned int)&b2,0);
	transfer_to_cache((unsigned int)&b3,0);
	transfer_to_cache((unsigned int)&c1,0);
	transfer_to_cache((unsigned int)&c2,0);
	transfer_to_cache((unsigned int)&c3,0);

	detx=finddet(d1,d2,d3,b1,b2,b3,c1,c2,c3);
	transfer_to_cache((unsigned int)&detx,1);

	transfer_to_cache((unsigned int)&a1,0);
	transfer_to_cache((unsigned int)&a2,0);
	transfer_to_cache((unsigned int)&a3,0);
	transfer_to_cache((unsigned int)&d1,0);
	transfer_to_cache((unsigned int)&d2,0);
	transfer_to_cache((unsigned int)&d3,0);
	transfer_to_cache((unsigned int)&c1,0);
	transfer_to_cache((unsigned int)&c2,0);
	transfer_to_cache((unsigned int)&c3,0);
	
	dety=finddet(a1,a2,a3,d1,d2,d3,c1,c2,c3);
	transfer_to_cache((unsigned int)&dety,1);

	transfer_to_cache((unsigned int)&a1,0);
	transfer_to_cache((unsigned int)&a2,0);
	transfer_to_cache((unsigned int)&a3,0);
	transfer_to_cache((unsigned int)&b1,0);
	transfer_to_cache((unsigned int)&b2,0);
	transfer_to_cache((unsigned int)&b3,0);
	transfer_to_cache((unsigned int)&d1,0);
	transfer_to_cache((unsigned int)&d2,0);
	transfer_to_cache((unsigned int)&d3,0);

	detz=finddet(a1,a2,a3,b1,b2,b3,d1,d2,d3);
	transfer_to_cache((unsigned int)&detz,1);
 
	transfer_to_cache((unsigned int)&detx,0);
	transfer_to_cache((unsigned int)&det,0);
	int x = detx/det;
	transfer_to_cache((unsigned int)&x,1);

	transfer_to_cache((unsigned int)&dety,0);
	transfer_to_cache((unsigned int)&det,0);
	int y = dety/det;
	transfer_to_cache((unsigned int)&y,1);

	transfer_to_cache((unsigned int)&detz,0);
	transfer_to_cache((unsigned int)&det,0);
	int z = detz/det;
	transfer_to_cache((unsigned int)&z,1);

	printf("\n\nSolution of Linear Eqution = (%d, %d, %d)\n",x,y,z);
	
	 
	printf("Data Read Hits = %d\n", data_cache->read_hits);
	printf("Data Write Hits = %d\n", data_cache->write_hits);
	printf("Data Read Misses = %d\n", data_cache->read_miss);
	printf("Data Write Misses = %d\n", data_cache->write_miss);
	float data_miss_rate = (float) (data_cache->read_miss+data_cache->write_miss) / (float) (data_cache->read_hits+data_cache->write_hits+data_cache->read_miss+ data_cache->write_miss);
	printf("Data Cache miss rate = %f %%\n",data_miss_rate * 100 );
	if(victim_size==0)
	printf("Total memory traffic to/from main memory = %d bytes\n\n",(int)(data_cache->read_miss+data_cache->write_miss) * sizeof(int));
	free(data_cache);
	
	if(victim_size != 0)
	{ 
	printf("Victim Read Hits = %d\n", victim_cache->read_hits);
	printf("Victim Write Hits = %d\n", victim_cache->write_hits);
	printf("Victim Read Misses = %d\n", victim_cache->read_miss);
	printf("Victim Write Misses = %d\n", victim_cache->write_miss);
	float victim_miss_rate = (float) (victim_cache->read_miss+victim_cache->write_miss) / (float) (victim_cache->read_hits+victim_cache->write_hits+victim_cache->read_miss+ victim_cache->write_miss);
	printf("Victim Cache miss rate = %f %%\n",victim_miss_rate * 100 );
	printf("Total memory traffic to/from main memory = %d bytes\n\n",(((data_cache->read_miss+data_cache->write_miss)-(victim_cache->read_hits+victim_cache->write_hits))* sizeof(int)));
	free(victim_cache);
	}
	return 0;
}

int finddet(int a1,int a2,int a3,int b1,int b2,int b3,int c1, int c2,int c3)
{
	int p1=0,p2=0,p3=0,p4=0,p5=0,p6=0;

	p1=a1*b2*c3;
	transfer_to_cache((unsigned int)&p1,1);
	p2=a1*b3*c2;
	transfer_to_cache((unsigned int)&p2,1);
	p3=a2*b1*c3;
	transfer_to_cache((unsigned int)&p3,1);
	p4=a3*b1*c2;
	transfer_to_cache((unsigned int)&p4,1);
	p5=a2*b3*c1;
	transfer_to_cache((unsigned int)&p5,1);
	p6=a3*b2*c1;
	transfer_to_cache((unsigned int)&p6,1);
	return ((p1)-(p2)-(p3)+(p4)+(p5)-(p6));
}

void decimal_binary(unsigned int n, int sel)
{
	int c,k;
for (c = 31; c >= 0; c--)
  {
    k = n >> c;
 
    if (k & 1)
     		bin[31-c] = '1';
    else
    		bin[31-c] = '0';
		
  }
	bin[32] = '\0';
	address_split(sel);
}

int binary_decimal(char* binary)
{
int i=0,len=0,dec=0,w;
len = strlen(binary);
for(i=0;i<len;i++)
{
w = pow(2,(len-i-1));
dec= dec + ((binary[i]-'0') * w);
}
return dec;

}

void address_split(int select)
{
	int cnt=0;
	int k=0;
	int tag_b=0,group_b=0;
	if (select == 1)
	{
		 tag_b = data_cache->tag_bits;
		 group_b = data_cache->group_bits;
	
	}
	if(select ==0)
	{
		 tag_b = victim_cache->tag_bits;
		 group_b = victim_cache->group_bits;
	}

			for(k=0;k<tag_b;k++)
					{
						tag[cnt] = bin[k];
						cnt++;
					}
			tag[cnt]='\0';
			cnt =0;
				for(k;k<(tag_b + group_b);k++)
					{
						group[cnt] = bin[k];
						cnt++;
					}
			group[cnt]='\0';
			cnt =0;
			/*for(k;k<(tag_b+group_b+offset_bits);k++)
					{
						offset[cnt] = bin[k];
						cnt++;
					}
			*/
			offset[cnt]='\0';
			cnt =0;
}

cache_stuct* cache_initialize(int cache_size)
{
	
	cache_stuct* cache;
	cache = (cache_stuct*) malloc(sizeof(cache_stuct));

	cache->read_miss = 0;
	cache->read_hits = 0;
	cache->write_hits = 0;
	cache->write_miss = 0;
		
	cache->size = cache_size;
	cache->block_size = block;
	cache->no_of_blocks = (int) cache_size/block;
	

	if (associativity == 1)
		set_way = 1 ;
	if (associativity == 2)
		set_way = 4;
	cache->no_of_sets = (int) cache->no_of_blocks/ set_way;
	cache->group_bits = (int) (log(cache->no_of_sets)/log(2));
	cache->tag_bits = (32 - cache->group_bits);
	
	int p =0;
	int q=0;
	cache->set = (set*) malloc(sizeof(block_info) * set_way);
	set* s;
	s = cache->set;
	s->set_next = NULL;
	block_info* blk;
	blk = (block_info*) s;
	
	while(s!=NULL && p < cache->no_of_sets)
		{
			q=0;	
			while(blk != NULL && q < set_way)
		{
			blk->count =0;
			blk->dirty = 0;
			blk->valid = 0;
			blk++;
			q++;
		}
		blk = (block_info*) malloc(sizeof(block_info) * set_way);
		s->set_next = (set*) blk;
		s = (set*) blk;
		s->set_next = NULL;
		p++;
		} 
	return cache;
}

void transfer_to_cache( unsigned int addr, int rw)
{
	decimal_binary(addr,1);
	int h=0;
	char* temp_tag;
	char block_1[33];
	temp_tag = (char*) malloc(sizeof(char*));	
	int dec_set = binary_decimal(group);
	strcpy(temp_tag,tag);
	set* temp_set = data_cache->set;
	block_info* temp_blk;
	for(int g =0; g<dec_set; g++)
		temp_set= temp_set->set_next;
	
	for(h=0; h<set_way;h++)
	{
		temp_blk = (block_info*) temp_set;
		if(h==0)
		{
			strcpy( block_1, temp_blk->block_tag);
			strcat(block_1,group);
		}
	
		if (temp_blk->block_tag != NULL)
		{
			
			if ( !(strcmp(temp_blk->block_tag,temp_tag)) && temp_blk->valid)//Check for hit/miss in Data Cache
			{
				
				if (rw ==0)
					data_cache->read_hits++;
				else
					data_cache->write_hits++;
					//printf("Data Cache Hit\n");
					break;
			}
		}
		temp_blk++;
	}

/*If it is a miss in Data Cache*/

	if (h==set_way)
	{		
			if (rw ==0)
				data_cache->read_miss++;
			else
				data_cache->write_miss++;
				//printf("Data Miss\n");
		if(victim_size ==0)								//Check if Victim Cache size is 0; If 0 place the requested block in Data Cache (replace if needed) 
		{
			int ct =0;
				temp_blk = (block_info*) temp_set;
			for (ct =0;ct<set_way;ct++)
			{
				if(temp_blk->valid != 1 )
				{					
					strcpy(temp_blk->block_tag,temp_tag);
					temp_blk->valid = 1;
					break;
				}
				temp_blk++;
			}
			if(ct==set_way)
			{
				temp_blk = (block_info*) temp_set;
				strcpy(temp_blk->block_tag,temp_tag);
				temp_blk->valid = 1;
			}
		}
		else	//If Victim Cache is non-zero, check for hit/miss in Victim Cache
		{
		decimal_binary(addr,0);
		h=0;
		char* temp_tag;
		temp_tag = (char*) malloc(sizeof(char*));
		int dec_set = binary_decimal(group);
		strcpy(temp_tag,tag);
		set* temp_set = victim_cache->set;
		block_info* temp_blk;
		for(int g =0; g<dec_set; g++)
			temp_set= temp_set->set_next;
		
		for(h=0; h<set_way;h++)
		{
		temp_blk = (block_info*) temp_set;
			if (temp_blk->block_tag != NULL)
				{
					if ( !(strcmp(temp_blk->block_tag,temp_tag)) && temp_blk->valid ) //Check for hit in Victim Cache
						{
							if (rw ==0)
								victim_cache->read_hits++;
							else
								victim_cache->write_hits++;
								//printf("Victim Hit\n");
								break;
						}
				}
			temp_blk++;
		}
/*If it is a miss in Victim Cache*/

		if(h==set_way)
		{
			if (rw ==0)
				victim_cache->read_miss++;
			else
				victim_cache->write_miss++;
				//printf("Victim Miss\n");
	
		decimal_binary(addr,1);	
		int h=0;
		char* temp_tag;
		temp_tag = (char*) malloc(sizeof(char*));	
		int dec_set = binary_decimal(group);
		strcpy(temp_tag,tag);
		set* temp_set = data_cache->set;
		block_info* temp_blk;
		for(int g =0; g<dec_set; g++)
			temp_set= temp_set->set_next;
	
	for(h=0; h<set_way;h++) 
	{
		temp_blk = (block_info*) temp_set;
			if(temp_blk->valid == 0)						//Put the requested block in vacant Data Cache block
				{
					if(rw==0)
						strcpy(temp_blk->block_tag,temp_tag);
					else
					{
						if (write ==1)
						strcpy(temp_blk->block_tag,temp_tag);
					}
					temp_blk->valid = 1;
					break;				
				}
		h++;
		temp_blk++;
	}
			if(h==set_way)
			{
				char temp_vic_tag[33],temp_vic_group[33];
				int n=0;
				int cn=0;
				for (cn=0;cn < (victim_cache->tag_bits);cn++)
				temp_vic_tag[cn] = block_1[cn];
				temp_vic_tag[cn] = '\0';
				for(cn;cn<32;cn++,n++)
				temp_vic_group[n] = block_1[cn];
				temp_vic_group[n] ='\0';
				int dec_set_temp = binary_decimal(temp_vic_group);
				set* temp_set = victim_cache->set;
				block_info* temp_blk;
				for(int g =0; g<dec_set_temp; g++)
				temp_set= temp_set->set_next;
				temp_blk = (block_info*) temp_set;
				for(h=0; h<set_way;h++)
				{
					if(temp_blk->valid == 0)		//Put the block replaced from Data Cache to vacant block in Victim Cache
					{
						if(rw==0)
						strcpy(temp_blk->block_tag ,temp_vic_tag);
						else
						{
							if(write == 1)
							strcpy(temp_blk->block_tag ,temp_vic_tag);
						}
						temp_blk->valid =1;
						break;
					}
					temp_blk++;
				}
				if(h==set_way)					//Put the block replaced from Data Cache to a block in Victim Cache
				{
					temp_blk = (block_info*) temp_set;
					if(rw==0)
						strcpy(temp_blk->block_tag ,temp_vic_tag);
					else
						{
							if(write==1)
							strcpy(temp_blk->block_tag ,temp_vic_tag);
						}
					temp_blk->valid =1;
				}

				set* temp_set_data = data_cache->set;
				block_info* temp_blk_data;
				for(int g =0; g<dec_set; g++)
				temp_set_data= temp_set_data->set_next;
				temp_blk_data = (block_info*) temp_set_data;
				strcpy(temp_blk_data->block_tag ,temp_tag); //Put the requested block in replaced Data Cache
				temp_blk_data->valid =1;
			}
		}
		}
	}
}


