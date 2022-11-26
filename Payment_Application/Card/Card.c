#include "Card.h"


static EN_cardError_t cardNameCheck(char name[]);
static EN_cardError_t cardDateCheck(char Date[]);
static EN_cardError_t cardPanCheck(char Pan[]);
void getCardHolderNameTest(void);

/*

	Ask for cardholder's name and store it into card data
	Card holder name is 24 characters string max and 20 min
	if cardholder name is null, less than 20 cahracters or more than 24 will return
	WRONG_NAME error, else return OK

*/
EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	EN_cardError_t stat = 0;
	uint8_t buffer[100];
	uint32_t len = 0;
	
	printf("Please Enter Your Card Name [20-24] characters : ");
	fflush(stdin);
	gets(buffer);
	fflush(stdin);
	stat =  cardNameCheck(buffer);
	if(stat == CARD_OK)
	{
		strcpy(cardData->cardHolderName, buffer);
	}
	else{}

	return stat;
}

/*

	will ask for the card expiry dateand store it in card data.
	Card expiry date is 5 characters string in the format "MM/YY", e.g "05/25".
	If the card expiry date is NULL, less or more than 5 characters, or has the wrong format will return WRONG_EXP_DATE error, else return OK.

*/
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	
	EN_cardError_t stat = 0;
	uint8_t  buffer[100];


	printf("Please Enter Your Card Expiry Date in [MM/YY] Formate : ");
	gets(buffer);


	//check the format of Expiry Date
	stat = cardDateCheck(buffer);

	if (stat == CARD_OK)
	{
		strcpy(cardData->cardExpirationDate, buffer);
	}
	else
	{
		/*return WRONG_CARD*/
	}
	
	return stat;
}



/*

	This function will ask for the card's Primary Account Number and store it in card data.
	PAN is 20 characters alphanumeric only string 19 character max, and 16 character min.
	If the PAN is NULL, less than 16 or more than 19 characters, will return WRONG_PAN error, else return OK.

	Give PAN that is a Luhn number, Luhn number generator, and checker

*/
EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	EN_cardError_t stat = 0;
	uint8_t buffer[100];
	uint32_t len = 0;
	uint8_t DoubleQoute[2] = {'"', '\0'};

	memset(buffer, 0, 100);
	memset(cardData->primaryAccountNumber, 0, 20);
	//put Double Qoutation before PAN Number
	strcat(cardData->primaryAccountNumber, DoubleQoute);

	printf("Please Enter Your PAN Number [16-19] characters : ");
	gets(buffer);

	stat = cardPanCheck(buffer);

	if (stat == CARD_OK)
	{
		strcat(cardData->primaryAccountNumber, buffer);
		//put Double Qoutation After PAN Number
		strcat(cardData->primaryAccountNumber, DoubleQoute);
	}
	else
	{
		/*return WRONG_CARD*/
	}

	return stat;
}


static EN_cardError_t cardNameCheck(char name[])
{
	EN_cardError_t stat = 0;
	uint32_t len = (uint32_t) strlen(name);
	int index = 0;

	if ( (!name) || (len < 20) || (len > 24) )//check if name length is NULL(0) OR less than 20 OR More Than 24
	{
		stat = WRONG_NAME;
	}
	else
	{
		while (name[index] != '\0')
		{
			//checkon all chars in string to verify that its alphabitc chars only
			if (((name[index] >= 'A') && (name[index] <= 'Z'))
				|| ((name[index] >= 'a') && (name[index] <= 'z')) || (name[index] == ' '))
			{
				/*Nice and continue check all chars*/
			}
			else
			{
				index = -1; // as indecation that the name is wrong
				break;
			}

			index++;
		}

		if (index == -1)//case wrong name
		{
			stat = WRONG_NAME;
		}
		else
		{
			stat = CARD_OK;
		}
	}

	return stat;
}

static EN_cardError_t cardDateCheck(char Date[])
{
	EN_cardError_t stat = 0;
	uint32_t len = 0;	

	//check if MM and YY is numeric
	if ((Date[0] >= '0') && (Date[0] <= '9') && (Date[1] >= '0') && (Date[1] <= '9')
	 && (Date[3] >= '0') && (Date[3] <= '9') && (Date[4] >= '0') && (Date[4] <= '9'))
	{
		len = (uint32_t) strlen(Date);
		//check if NULL, Not Equal 5 OR the middel char is not '/'
		if ((!Date) || (len != 5) || (Date[2] != '/')) 
		{
			stat = WRONG_EXP_DATE;
		}
		//check if Month > 12
		else if ((Date[0] > '1') || ((Date[0] == '1') && (Date[1] > '2')))
		{
			stat = WRONG_EXP_DATE;
		}
		else
		{
			stat = CARD_OK;
		}
	}
	else
	{
		stat = WRONG_EXP_DATE;
	}

	

	return stat;
}

static EN_cardError_t cardPanCheck(char Pan[])
{
	EN_cardError_t stat = 0;
	uint32_t len = 0;
	int index = 0;

	while (Pan[index] != '\0')
	{
		//check all chars that it is numeric
		if ( ((Pan[index] >= '0') && (Pan[index] <= '9')) || (Pan[index] <= ' '))
		{
			/*nice and continue*/
		}
		else
		{
			index = -1;//as indecation of non numeric char and break this loop
			break;
		}

		index++;
	}

	len = (uint32_t) strlen(Pan);

	if ( (!Pan) || (index == -1) || (len > 19) || (len < 16))
	{
		stat = WRONG_PAN;
	}
	else
	{
		stat = CARD_OK;
	}

	return stat;
}


void getCardHolderNameTest(void)
{
	ST_cardData_t cardDataTest;
	EN_cardError_t stat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: getCardHolderName\n");
	
	//case 1: NULL
	printf("Test Case 1:\n");
	printf("Input Data: NULL\n");

	stat  = getCardHolderName(&cardDataTest);

	printf("Expected Result: WRONG_NAME\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_NAME\n");
	}

	printf("-----------------------------------------------\n");
	//case 2: Numeric chars in name
	printf("Test Case 2:\n");
	printf("Input Data: hossam mas99oud elzhar\n");

	stat = getCardHolderName(&cardDataTest);

	printf("Expected Result: WRONG_NAME\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_NAME\n");
	}

	printf("-----------------------------------------------\n");
	//case 3:  < 20 
	printf("Test Case 3:\n");
	printf("Input Data: hossam\n");

	stat = getCardHolderName(&cardDataTest);

	printf("Expected Result: WRONG_NAME\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_NAME\n");
	}

	printf("-----------------------------------------------\n");
	//case 4:  > 24
	printf("Test Case 4:\n");
	printf("Input Data: hossam masoud abdelmordy elzhar\n");

	stat = getCardHolderName(&cardDataTest);

	printf("Expected Result: WRONG_NAME\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_NAME\n");
	}

	printf("-----------------------------------------------\n");
	//case 5: happy case 
	printf("Test Case 5:\n");
	printf("Input Data: hossam masoud elzhar\n");

	stat = getCardHolderName(&cardDataTest);

	printf("Expected Result: CARD_OK\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_NAME\n");
	}

	printf("================================================\n\n");
}


void getCardExpiryDateTest(void)
{
	ST_cardData_t cardDataTest;
	EN_cardError_t stat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: getCardExpiryDate\n");

	//case 1: NULL
	printf("Test Case 1:\n");
	printf("Input Data: NULL\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: WRONG_EXP_DATE\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("-----------------------------------------------\n");
	//case 2: Alphabitc chars
	printf("Test Case 2:\n");
	printf("Input Data: 9A/B2\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: WRONG_EXP_DATE\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("-----------------------------------------------\n");
	//case 3: Middle char is not '/' 
	printf("Test Case 3:\n");
	printf("Input Data: 10523\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: WRONG_EXP_DATE\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("-----------------------------------------------\n");
	//case 4: less than 5 chars
	printf("Test Case 4:\n");
	printf("Input Data: 10/5\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: WRONG_EXP_DATE\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("-----------------------------------------------\n");
	//case 5: more than 5 chars
	printf("Test Case 5:\n");
	printf("Input Data: 10/250\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: WRONG_EXP_DATE\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("-----------------------------------------------\n");
	//case 6: Month > 12
	printf("Test Case 5:\n");
	printf("Input Data: 22/22\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: WRONG_EXP_DATE\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("-----------------------------------------------\n");

	//case 7: happy case 
	printf("Test Case 6:\n");
	printf("Input Data: 12/25\n");

	stat = getCardExpiryDate(&cardDataTest);

	printf("Expected Result: CARD_OK\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_EXP_DATE\n");
	}

	printf("================================================\n\n");
}


void getCardPANTest(void)
{
	ST_cardData_t cardDataTest;
	EN_cardError_t stat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: getCardPAN\n");

	//case 1: NULL
	printf("Test Case 1:\n");
	printf("Input Data: NULL\n");

	stat = getCardPAN(&cardDataTest);

	printf("Expected Result: WRONG_PAN\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_PAN\n");
	}

	printf("-----------------------------------------------\n");
	//case 2: Alphabitc chars
	printf("Test Case 2:\n");
	printf("Input Data: 4456 55ab za24 5556\n");

	stat = getCardPAN(&cardDataTest);

	printf("Expected Result: WRONG_PAN\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_PAN\n");
	}

	printf("-----------------------------------------------\n");
	//case 3:  less than 16 chars 
	printf("Test Case 3:\n");
	printf("Input Data: 4426 5594 11\n");

	stat = getCardPAN(&cardDataTest);

	printf("Expected Result: WRONG_PAN\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_PAN\n");
	}

	printf("-----------------------------------------------\n");
	//case 4:  more than 19 chars
	printf("Test Case 4:\n");
	printf("Input Data: 4485 6695 88882 11254\n");

	stat = getCardPAN(&cardDataTest);

	printf("Expected Result: WRONG_PAN\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_PAN\n");
	}

	printf("-----------------------------------------------\n");
	//case 5: happy case 
	printf("Test Case 5:\n");
	printf("Input Data: 4428 1120 5241 9785\n");

	stat = getCardPAN(&cardDataTest);

	printf("Expected Result: CARD_OK\n");
	if (stat == CARD_OK)
	{
		printf("Actual Result: CARD_OK\n");
	}
	else
	{
		printf("Actual Result: WRONG_PAN\n");
	}

	printf("==================================================\n\n");
}