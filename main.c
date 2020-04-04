#include<stdio.h>
#include<stdio_ext.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


typedef struct shopping
{
    char name[100];
    char phone[11];
}customer;

typedef struct item
{
    char code[20];
    char name[20];
    int sno;
    float rate;
    float quantity;
    float price;
    float discount;
    float tax;
}items;


float TOTAL=0;
float DISCOUNT=0;
float TAX=0;
float AMOUNT=0;
int ITEM_COUNT=0;

//customerdb
void login( FILE*, customer*);
int validate( FILE*, customer*);
void create_User( FILE*, customer*);
void disp_User( FILE*, customer*);
void edit_User(customer*);



//bill
void bill(customer*);
void generate_bill(char*,customer*,char*);
int check_item(items*);
void display_bill(FILE*,customer*,char*,char*);

int main()
{
    FILE *fp1;
    fp1 = fopen("CustomerDB","a+");

    static int choice=1;

    customer *det = malloc(sizeof(customer));


    do
    {
        //system("clear");
        printf("WELCOME TO XYZ SUPERMARKET\n");
        printf("PRESS 1 TO CONTINUE\n");
        printf("PRESS 0 TO EXIT\n");
        scanf("%d", &choice);
        if (choice)
        {
            login(fp1,det);
        }
        
          
    } while (choice);
    

}


void login(FILE *fp, customer *det)
{
    system("clear");
    printf("Enter the phone number:");
    scanf("%s",det->phone);
    
    if(validate( fp, det))
    {
        printf("\nWelcome %s\n",det->name);
        printf("DO YOU WANT TO EDIT YOUR PERSONAL DETAILS OR PROCEED TO BILLING\n");
        printf("ENTER 1 TO EDIT\nENTER 0 TO PROCEED\n");
        int choice;
        scanf("%d",&choice);
        if(choice)
        {
            
            edit_User(det);
        }      
        bill(det);                
    }

    else
    {
        create_User(fp,det);
    }
    
}


int validate( FILE *fp, customer *det)
{
    customer *new = malloc(sizeof(customer));
    fseek(fp , 0 ,SEEK_SET);
    int result=0;
    while (!feof(fp))
    {
        //fscanf(fp,"%s %s",new->name,new->phone);
        fread(new,sizeof (customer),1,fp);
        if(!strcmp(det->phone,new->phone))
        {
            strcpy(det->name,new->name);
            printf("Valid \n");
            result = 1;
            break;
            
        }      
        
    }

    free(new);
    return result;
}


void create_User( FILE* fp, customer* det)
{
    printf("Enter the customer name:");
    scanf("%s",det->name);
    __fpurge(stdin);
    fseek(fp , sizeof(customer) ,SEEK_END);
    //fprintf(fp,"%s %s\n",det->phone,det->name);
    fwrite(det,sizeof(customer),1,fp);
}


void disp_User( FILE* fp, customer* det)
{
    customer *new = malloc(sizeof(customer));
    fseek(fp, 0, SEEK_SET);
    int i=1;
    while (!feof(fp))
    {
        fread(new,sizeof (customer),1,fp);
        printf("%d %s %s\n",i,new->name,new->phone);
        i++;

    }
    
}


void edit_User(customer* det)
{
    customer *new = malloc(sizeof(customer));
    
    FILE *fp;
    fp = fopen("CustomerDB","r+");

    int pos=0;
    fseek(fp,0,SEEK_SET);
    while (!feof(fp))
    {
        fread(new,sizeof(customer),1,fp);
        if(!strcmp(det->phone,new->phone))
        {
            break;            
        }
        pos++;
    }
    printf("%d\n",pos);
    fseek(fp,sizeof(customer)*pos,SEEK_SET);
    fread(new,sizeof(customer),1,fp);

    printf("Old details:\n%s %s\n",new->name,new->phone);

    printf("Enter new Phone number:");
    scanf("%s",new->phone);
    __fpurge(stdin);
    printf("Enter new Name:");
    scanf("%s",new->name);
    __fpurge(stdin);

    
    fseek(fp,sizeof(customer)*pos,SEEK_SET);
    fwrite(new,sizeof(customer),1,fp);
    free(new);
    fclose(fp);
    
}





//BILL

void bill(customer *det)
{
    __fpurge(stdin);
    
    char *date = malloc(sizeof(char)*20);

    printf("%s\n",date);

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char d[3];char m[3];char y[5];
    static char str[20];
    sprintf(d, "%d", timeinfo->tm_mday);
    sprintf(y, "%d", timeinfo->tm_year + 1900);
    sprintf(m, "%d", timeinfo->tm_mon + 1);
    
    strcpy(date,d);
    strcat(date,"|");
    strcat(date,m);
    strcat(date,"|");
    strcat(date,y);
    printf("%s\n",date);

    char dup[20];
    strcpy(dup,date);
    char *filename = strcat(dup,".txt");

    FILE *f;
    f = fopen(filename,"a+");

    char invoiceno[10];
    strcpy(invoiceno,date);
    strcat(invoiceno,"-");
    char dump[20];
    int count=0;
    //fseek(fp2 , 0 ,SEEK_SET);
    while (!feof(f))
    {
        fscanf(f,"%s",dump);
        count++;
        printf("%s %d\n",dump,count);
    }
    char c[5];
    sprintf(c,"%d",count);
    strcat(invoiceno,c);
    fprintf(f,"%s\n",invoiceno);

    generate_bill(invoiceno,det,date);

    fclose(f);
    free(date);

    
}

void generate_bill(char *invoice_no,customer *det,char *date)
{
    system("clear");
    FILE *temp_bill_ptr = fopen("Temp","a+");
    items *entry = malloc(sizeof(items));

    int choice=1;
    //int count=1;
    do
    {
        entry->sno = ITEM_COUNT+1;
        printf("\n\nEnter the item code: ");
        scanf("%s",entry->code);
        if(check_item(entry))
        {
            printf("Enter the item quantity: ");
            scanf("%f",&entry->quantity);
            entry->price = entry->rate * entry->quantity;
            entry->tax = 0.18*entry->price;
            ITEM_COUNT++;
            printf("\nPress 0 to stop entering\nPress 1 to continue entering\n");
            fwrite(entry,sizeof(items),1,temp_bill_ptr);
            scanf("%d",&choice);
        }
        else
        {
            printf("\nPlease Enter Correct Code\n");
        }
        
    } while (choice);
    
    display_bill(temp_bill_ptr, det,invoice_no,date);
    printf("");

}

int check_item(items* entry)
{
    FILE *fp = fopen("ITEMS.txt","r+");
    items *temp =malloc(sizeof(items));
    rewind(fp);
    while (!feof(fp))
    {
        fscanf(fp,"%s %s %f %f ",&temp->code,&temp->name,&temp->rate,&temp->discount);
        if(!strcmp(entry->code,temp->code))
        {
            strcpy(entry->name,temp->name);
            entry->rate = temp->rate;
            entry->discount = temp->discount;
            entry->tax = 0.18*temp->tax;
            fclose(fp);
            return 1;

        }
        
    }
    fclose(fp);
    return 0;
    
}

void display_bill(FILE *fp,customer *det,char *invoice_no,char *date)
{
    items *temp =malloc(sizeof(items));
    rewind(fp);
    system("clear");
    printf("*********************************************************************************************************************************************************************************************\n");
    printf("\t\t\t\t\t\t\t\t\t\tAVENUE SUPERMARKET\n");
    printf("\t\t\t\t\t\t\t\t\t    SURVEY NO 21, PLOT NO 22\n");
    printf("\t\t\t\t\t\t\t\t\t ELECTRONIC CITY,BANGALORE SOUTH\n");
    printf("\t\t\t\t\t\t\t\t\t\tBANGALORE 560100\n");
    printf("\t\t\t\t\t\t\t\t\t\tPHONE: 08028520303\n");
    printf("*********************************************************************************************************************************************************************************************\n");
    printf("\t\t\t\t\t\t\t\t\t\tTAX INVOICE\n");
    printf("INVOICE NO: %-12s\t\t\t\t\t\t\t\t\t\t\tCUSTOMER NAME: %s\n",invoice_no,det->name);
    printf("BILL DATE: %-10s\t\t\t\t\t\t\t\t\t\t\t\tCUSTOMER NUMBER:%s\n",date,det->phone);
    printf("*********************************************************************************************************************************************************************************************\n");
    printf("SERIAL NO\t\t\tITEM CODE\t\t\tITEM NAME      \t\t\tQUANTITY	\t\t\tRATE   \t\t\t\tVALUE\n");
    printf("*********************************************************************************************************************************************************************************************\n");
    
    for( int i =0;i<ITEM_COUNT;i++)
    {
        fread(temp,sizeof(items),1,fp);
        TOTAL = TOTAL + temp->price;
        TAX += temp->tax;
        DISCOUNT += (temp->price + temp->tax)*(temp->discount)/100;
        printf("%-9d\t\t\t%-9s\t\t\t%-15s\t\t\t%-11.2f\t\t\t\t%-7.2f\t\t\t\t%.2f\n",temp->sno,temp->code,temp->name,temp->quantity,temp->rate,temp->price);
    }

    printf("*********************************************************************************************************************************************************************************************\n");
    printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  SUB-TOTAL  :%.2f\n",TOTAL);
    printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  TAX @ 18%   :%.2f\n",TAX);
    AMOUNT = TOTAL + TAX;
    printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  TOTAL      :%.2f\n",AMOUNT);
    printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  DISCOUNT   :%.2f\n",DISCOUNT);
    AMOUNT -= DISCOUNT ;
    printf("*********************************************************************************************************************************************************************************************\n");
    printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  GRAND TOTAL:%.2f\n",AMOUNT);
    printf("*********************************************************************************************************************************************************************************************\n");
    fclose(fp);
    remove("Temp");
}
