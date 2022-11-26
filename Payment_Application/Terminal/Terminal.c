#include "Terminal.h"


EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	uint8_t   day[3];
	uint8_t   month[3];
	uint8_t   year[5];
	uint8_t   date[11] = { '\0' };
	uint8_t   slash[2] = { '/','\0' };

	time_t rawtime = time(NULL);
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);

	sprintf(day, "%d", timeinfo.tm_mday);
	sprintf(month, "%d", (timeinfo.tm_mon + 1));
	sprintf(year, "%d", (timeinfo.tm_year + 1900));

	strcat(date, day);
	strcat(date, slash);
	strcat(date, month);
	strcat(date, slash);
	strcat(date, year);

	strcpy(termData->transactionDate, date);

	return TERMINAL_OK;
}


EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{
	EN_terminalError_t stat = 0;
	uint32_t cardMonth  = 0;
	uint32_t cardYear   = 0;
	uint32_t  day = 0;
	uint32_t  month = 0;
	uint32_t  year = 0;
	
	cardMonth = (uint32_t)(((cardData->cardExpirationDate[0] - '0') * 10) + (cardData->cardExpirationDate[1] - '0'));
	cardYear = (uint32_t)(((cardData->cardExpirationDate[3] - '0') * 10) + (cardData->cardExpirationDate[4] - '0'));
	
	month = (uint32_t)(((termData->transactionDate[3] - '0') * 10) + (termData->transactionDate[4] - '0'));
	year  = (uint32_t)(((termData->transactionDate[8] - '0') * 10) + (termData->transactionDate[9] - '0'));

	if (cardYear < year)
	{
		stat = EXPIRED_CARD;
	}
	else if ((cardYear == year) && (cardMonth < month)) //if the same year, we need to check the month
	{
		stat = EXPIRED_CARD;
	}
	else
	{
		stat = TERMINAL_OK;
	}

	return stat;
	
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	EN_terminalError_t stat = 0;
	float amount = 0;

	printf("Please Enter The Transaction Amount : ");
	
	fflush(stdin);
	scanf("%f", &amount);

	if (amount <= 0)
	{
		stat = INVALID_AMOUNT;
	}
	else
	{
		stat = TERMINAL_OK;
		termData->transAmount = amount;
	}

	return stat;
}


EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount)
{
	EN_terminalError_t stat = 0;

	if (maxAmount <= 0)
	{
		stat = INVALID_MAX_AMOUNT;
	}
	else
	{
		stat = TERMINAL_OK;
		termData->maxTransAmount = maxAmount;
	}

	return stat;
}


EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	EN_terminalError_t stat = 0;

	if ((termData->transAmount) > (termData->maxTransAmount))
	{
		stat = EXCEED_MAX_AMOUNT;
	}
	else
	{
		stat = TERMINAL_OK;
	}

	return stat;
}



//testing section

void getTransactionDateTest(void)
{
	//NoThing to test, this Function gets Transaction Date From System
}


void isCardExpriedTest(void)
{
	ST_terminalData_t terminalDataTest;
	ST_cardData_t cardDataTest;
	EN_terminalError_t stat = 0;

	strcpy(terminalDataTest.transactionDate, "10/26/2022");

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: isCardExpried\n");

	//case 1: Exepired
	printf("Test Case 1:\n");
	printf("Input Data: 02/20\n");
	strcpy(cardDataTest.cardExpirationDate, "02/20");
	stat = isCardExpired(&cardDataTest, &terminalDataTest);

	printf("Expected Result: EXPIRED_CARD\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: EXPIRED_CARD\n");
	}

	printf("-----------------------------------------------\n");
	//case 2: NOT EXepired
	printf("Test Case 1:\n");
	printf("Input Data: 02/23\n");
	strcpy(cardDataTest.cardExpirationDate, "02/23");

	stat = isCardExpired(&cardDataTest, &terminalDataTest);

	printf("Expected Result: TERMINAL_OK\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: EXPIRED_CARD\n");
	}

	printf("================================================\n\n");
}

void getTransactionAmountTest(void)
{
	ST_terminalData_t terminalDataTest;
	EN_terminalError_t stat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: getTransactionAmount\n");

	//case 1: less than zero
	printf("Test Case 1:\n");
	printf("Input Data: -2000\n");
	stat = getTransactionAmount(&terminalDataTest);

	printf("Expected Result: INVALID_AMOUNT\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: INVALID_AMOUNT\n");
	}

	printf("-----------------------------------------------\n");
	//case 2: zero
	printf("Test Case 2:\n");
	printf("Input Data: 0\n");
	stat = getTransactionAmount(&terminalDataTest);

	printf("Expected Result: INVALID_AMOUNT\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: INVALID_AMOUNT\n");
	}

	printf("-----------------------------------------------\n");
	//case 3: more than zero
	printf("Test Case 3:\n");
	printf("Input Data: 500\n");
	stat = getTransactionAmount(&terminalDataTest);

	printf("Expected Result: TERMINAL_OK\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: INVALID_AMOUNT\n");
	}

	printf("================================================\n\n");
}


void setMaxAmountTest(void)
{
	ST_terminalData_t terminalDataTest;
	EN_terminalError_t stat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: setMaxAmount\n");

	//case 1: less than zero
	printf("Test Case 1:\n");
	printf("Input Data: -2000\n");
	stat = setMaxAmount(&terminalDataTest, -2000);

	printf("Expected Result: INVALID_MAX_AMOUNT\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: INVALID_MAX_AMOUNT\n");
	}

	printf("-----------------------------------------------\n");
	//case 2: zero
	printf("Test Case 2:\n");
	printf("Input Data: 0\n");
	stat = setMaxAmount(&terminalDataTest, 0);

	printf("Expected Result: INVALID_MAX_AMOUNT\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: INVALID_MAX_AMOUNT\n");
	}

	printf("-----------------------------------------------\n");
	//case 3: more than zero
	printf("Test Case 3:\n");
	printf("Input Data: 50000\n");
	stat = setMaxAmount(&terminalDataTest, 50000);

	printf("Expected Result: TERMINAL_OK\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: INVALID_MAX_AMOUNT\n");
	}

	printf("================================================\n\n");
}

void isBelowMaxAmountTest(void)
{
	ST_terminalData_t terminalDataTest;
	EN_terminalError_t stat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: isBelowMaxAmount\n");

	terminalDataTest.maxTransAmount = 5000.0;
	terminalDataTest.transAmount = 6000.0;

	//case 1: trans amount > max amount
	printf("Test Case 1:\n");
	printf("Input Data: 6000.0, max amount = 5000.0\n");
	stat = isBelowMaxAmount(&terminalDataTest);

	printf("Expected Result: EXCEED_MAX_AMOUNT\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: EXCEED_MAX_AMOUNT\n");
	}

	printf("-----------------------------------------------\n");

	//case 2: trans amount < max amount
	terminalDataTest.maxTransAmount = 5000.0;
	terminalDataTest.transAmount = 2000.0;

	printf("Test Case 2:\n");
	printf("Input Data: 2000.0, max amount = 5000.0\n");
	stat = isBelowMaxAmount(&terminalDataTest);

	printf("Expected Result: TERMINAL_OK\n");
	if (stat == TERMINAL_OK)
	{
		printf("Actual Result: TERMINAL_OK\n");
	}
	else
	{
		printf("Actual Result: EXCEED_MAX_AMOUNT\n");
	}

	printf("================================================\n\n");
}
