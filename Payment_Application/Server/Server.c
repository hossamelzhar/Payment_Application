#include "Server.h"

ST_accountsDB_t accountsDB[255];
ST_transaction transctionDB[255];

uint32_t sequanceNumber = 0;

static int getLastTransactionSeqNum(void);

void getAccountsDB(void)
{
	float balance = 0;
	uint8_t stat[10];
	uint8_t pan[20];
	uint32_t index = 0;

	printf("Reading Accounts Data Base ... \n");

	FILE* ptr = fopen("../AcountsDB.txt", "r");
	if (ptr == NULL)
	{
		printf("failed to open The  Accounts DataBase File\n");
	}
	else
	{
		while (fscanf(ptr, "%f %s %s", &balance, stat, pan) != EOF)
		{
			accountsDB[index].balance = balance;
			if (strcmp(stat, "RUNNING") == 0)
			{
				accountsDB[index].state = RUNNING;
			}
			else if (strcmp(stat, "BLOCKED") == 0)
			{
				accountsDB[index].state = BLOCKED;
			}
			strcpy((accountsDB[index].primaryAccountNumber), pan);
			index++;
		}

		accountsDB[index].primaryAccountNumber[0] = '\0'; //as termination

		fclose(ptr);
		printf("Accounts Data Base Succesfully loaded.");
	}
}



EN_transStat_t recieveTransactionData(ST_transaction* transData)
{
	EN_transStat_t stat = APPROVED;
	ST_accountsDB_t accountRefrence;
	
	if (isValidAccount(&(transData->cardHolderData), &accountRefrence) == ACCOUNT_NOT_FOUND)
	{
		transData->transState = FRAUD_CARD;
		stat = FRAUD_CARD;
	}
	else if (isAmountAvailable(&(transData->terminalData), &accountRefrence) == LOW_BALANCE)
	{
		transData->transState = DECLINED_INSUFFECIENT_FUND;
		stat = DECLINED_INSUFFECIENT_FUND;
	}
	else if (isBlockedAccount(&accountRefrence) == BLOCKED_ACCOUNT)
	{
		transData->transState = DECLINED_STOLEN_CARD;
		stat = DECLINED_STOLEN_CARD;
	}
	else
	{
		transData->transState = APPROVED;
		stat = APPROVED;
	}

	//update balance value if transaction APPROVED
	if(stat == APPROVED)
	{
		accountRefrence.balance -= transData->terminalData.transAmount;
		int index = 0;
		while (accountsDB[index].primaryAccountNumber[0] != '\0')
		{
			if (strcmp((accountsDB[index].primaryAccountNumber), (accountRefrence.primaryAccountNumber)) == 0)
			{
				accountsDB[index].balance = accountRefrence.balance;
				break;
			}
			else
			{
				index++;
			}
		}
	}
	else{}
	
	return stat;
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	EN_serverError_t RetStat = 0;
	uint32_t index = 0;
	
	if ((NULL == cardData) || (NULL == accountRefrence))
	{
		RetStat = -1;
	}
	else
	{
		while ((accountsDB[index].primaryAccountNumber[0]) != '\0')
		{

			if (strcmp((cardData->primaryAccountNumber), (accountsDB[index].primaryAccountNumber)) == 0)
			{
				accountRefrence->balance = accountsDB[index].balance;
				accountRefrence->state = accountsDB[index].state;
				strcpy(accountRefrence->primaryAccountNumber, accountsDB[index].primaryAccountNumber);
				RetStat = SERVER_OK;
				break;
			}
			else
			{
				RetStat = ACCOUNT_NOT_FOUND;
			}

			index++;
		}
	}

	return RetStat;
}

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if (NULL == accountRefrence)
	{
		return -1;
	}
	else
	{
		if ((accountRefrence->state) == BLOCKED)
		{
			return BLOCKED_ACCOUNT;
		}
		else
		{
			return SERVER_OK;
		}
	}
	
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{
	if( (NULL == termData) || (NULL == accountRefrence) )
	{
		return -1;
	}
	else
	{
		if ((termData->transAmount) > (accountRefrence->balance))
		{
			return LOW_BALANCE;
		}
		else
		{
			return SERVER_OK;
		}
	}
	
}

EN_serverError_t saveTransaction(ST_transaction* transData)
{
	int lastSeqNum = getLastTransactionSeqNum();
	uint8_t stat[30] = {'\0'};

	lastSeqNum++;
	transData->transactionSequenceNumber = lastSeqNum;

	if (transData->transState == APPROVED)
	{
		strcpy(stat, "APPROVED");
	}
	else if (transData->transState == DECLINED_INSUFFECIENT_FUND)
	{
		strcpy(stat, "DECLINED_INSUFFECIENT_FUND");
	}
	else if (transData->transState == DECLINED_STOLEN_CARD)
	{
		strcpy(stat, "DECLINED_STOLEN_CARD");
	}
	else if (transData->transState == FRAUD_CARD)
	{
		strcpy(stat, "FRAUD_CARD");
	}
	else{}

	FILE* ptr = fopen("../TransactionsDB.txt", "a");
	if (ptr == NULL)
	{
		printf("failed to open The  Transactions DataBase File\n");
		return SAVING_FAILED;
	}
	else
	{
		printf("Saving Transaction ...\n");
		fprintf(ptr, "%d %s %.1f %s %.1f %s %s % s\n", lastSeqNum, transData->terminalData.transactionDate,
													transData->terminalData.transAmount,
													stat,
													transData->terminalData.maxTransAmount,
													transData->cardHolderData.cardHolderName,
													transData->cardHolderData.primaryAccountNumber,
													transData->cardHolderData.cardExpirationDate    );
		
		fclose(ptr);

		return SERVER_OK;
	}

}

void listSavedTransactions(void)
{
	uint32_t seqNum = 0;
	uint8_t transDate[11];
	float transAmount = 0;
	uint8_t transStat[30];
	float maxAmount = 0;
	uint8_t cardName[4][10];
	uint8_t cardPAN[20];
	uint8_t expirDate[10];

	FILE* ptr = fopen("../TransactionsDB.txt", "r");
	if (ptr == NULL)
	{
		printf("failed to open The  Transactions DataBase File\n");
		
	}
	else
	{
		while (fscanf(ptr, "%d %s %f %s %f %s %s %s %s %s", &seqNum, transDate, &transAmount, transStat, &maxAmount,
			cardName[0], cardName[1], cardName[2], cardPAN, expirDate) != EOF)
		{
			printf("#####################################\n");
			printf("Transaction Sequence Number: %d\n", seqNum);
			printf("Transaction Date: %s\n", transDate);
			printf("Transaction Amount: %.1f\n", transAmount);
			printf("Transaction State: %s\n", transStat);
			printf("Terminal Max Amount: %.1f\n", maxAmount);
			printf("Cardholder Name: %s %s %s\n", cardName[0], cardName[1], cardName[2]);
			printf("PAN: %s\n", cardPAN);
			printf("Card Expiration Date: %s\n", expirDate);
			printf("#####################################\n");
		}

		fclose(ptr);
	}
}

static int getLastTransactionSeqNum(void)
{
	int lastSeqNum = -1; // as defualt
	uint8_t tempLine[200] = { '\0' };
	uint32_t index = 0;

	FILE* ptr = fopen("../TransactionsDB.txt", "r");
	if (ptr == NULL)
	{
		return lastSeqNum;
	}
	else
	{
		//get last sequace number
		fscanf(ptr, "%d", &lastSeqNum);

		while (fscanf(ptr, "%[^\n]s", tempLine) != EOF)
		{
			fscanf(ptr, "%d", &lastSeqNum);
		}

		fclose(ptr);
	}
	return lastSeqNum;
}


void updateAccountsDB(void)
{
	uint32_t index = 0;
	uint8_t stat[10] = { '\0' };

	FILE* ptr = fopen("../AcountsDB.txt", "w");
	if (ptr == NULL)
	{
		printf("failed to open The DataBase File\n");
	}
	else
	{
		while (accountsDB[index].primaryAccountNumber[0] != '\0')
		{
			if (accountsDB[index].state == RUNNING)
			{
				strcpy(stat, "RUNNING");
			}
			else if (accountsDB[index].state == BLOCKED)
			{
				strcpy(stat, "BLOCKED");
			}
			fprintf(ptr, "%.1f %s %s\n", accountsDB[index].balance, stat, accountsDB[index].primaryAccountNumber);
			index++;
		}

		fclose(ptr);
	}
}



/******************************** Test Section *********************************/

void isValidAccountTest(void)
{
	ST_cardData_t cardData;
	ST_accountsDB_t accountRefrence;
	EN_serverError_t RetStat = 0;

	uint8_t buffer[25];

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: isValidAccount\n");

	
	printf("\n\ncase 1 : NOT VALID\n");
	printf("Input Data : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);

	RetStat = isValidAccount(&cardData, &accountRefrence);

	printf("Expected Result : ACCOUNT_NOT_FOUND\n");
	if (ACCOUNT_NOT_FOUND == RetStat)
	{
		printf("Actual Result : ACCOUNT_NOT_FOUND\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}

	printf("\ncase 2 : VALID\n");
	printf("Input Data : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);
	RetStat = isValidAccount(&cardData, &accountRefrence);
	
	printf("Expected Result : SERVER_OK\n");
	if (ACCOUNT_NOT_FOUND == RetStat)
	{
		printf("Actual Result : ACCOUNT_NOT_FOUND\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}
	printf("=====================================================\n");
	getchar();
	
}

void isBlockedAccountTest(void)
{
	ST_accountsDB_t accountRefrence;
	EN_serverError_t RetStat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: isBlockedAccount\n");

		
	printf("\n\ncase 1 : BLOCKED ACCOUNT\n");
	printf("user_input : BLOCKED\n");
	accountRefrence.state = BLOCKED;
	RetStat = isBlockedAccount(&accountRefrence);

	printf("Expected Result : BLOCKED_ACCOUNT\n");
	if (BLOCKED_ACCOUNT == RetStat)
	{
		printf("Actual Result : BLOCKED_ACCOUNT\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}

	getchar();

	printf("\ncase 2 : RUNNING ACCOUNT\n");
	printf("user_input : RUNNING\n");
	accountRefrence.state = RUNNING;
	RetStat = isBlockedAccount(&accountRefrence);

	printf("Expected Result : SERVER_OK\n");
	if (BLOCKED_ACCOUNT == RetStat)
	{
		printf("Actual Result : BLOCKED_ACCOUNT\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}
	printf("=====================================================\n");
	getchar();

}

void isAmountAvailableTest(void)
{
	ST_terminalData_t termData;	
	ST_accountsDB_t accountRefrence;
	EN_serverError_t RetStat = 0;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: isAmountAvailable\n");

	accountRefrence.balance = 2000;
	printf("\n\nAccount balance  = 2000\n");

	float amount;

	
	printf("\ncase 1 : LOW BALANCE\n");
	printf("Enter transaction Amount : ");
	scanf("%f", &amount);
	termData.transAmount = amount;
	
	RetStat = isAmountAvailable(&termData , &accountRefrence);

	printf("Expected Result : LOW_BALANCE\n");
	if (LOW_BALANCE == RetStat)
	{
		printf("Actual Result : LOW_BALANCE\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}


	printf("\ncase 2 : enough balance\nAccount balance  = 2000\n");
	printf("Enter transaction Amount : ");
	scanf("%f", &amount);
	termData.transAmount = amount;

	RetStat = isAmountAvailable(&termData, &accountRefrence);

	printf("Expected Result : SERVER_OK\n");
	if (LOW_BALANCE == RetStat)
	{
		printf("Actual Result : LOW_BALANCE\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}

	printf("=====================================================\n");
	getchar();
}


void recieveTransactionDataTest(void)
{
	ST_cardData_t cardData;
	ST_terminalData_t terminalData;
	ST_transaction transactionData;
	EN_transStat_t RetStat;
	uint8_t buffer[30];
	float amount;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: recieveTransactionData\n");

	/************************************* case 1 *********************************/
	printf("\n\ncase 1 : NOT Valid Account\n");

	printf("Card Primary Account Number : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);

	printf("Trans Amount : ");
	scanf("%f", &amount);
	terminalData.transAmount = amount;

	transactionData.cardHolderData = cardData;
	transactionData.terminalData = terminalData;

	RetStat = recieveTransactionData(&transactionData);

	printf("Expected Result : FRAUD_CARD\n");

	if (FRAUD_CARD == RetStat)
	{
		printf("Actual Result : FRAUD_CARD\n");
	}
	else if (DECLINED_INSUFFECIENT_FUND == RetStat)
	{
		printf("Actual Result : DECLINED_INSUFFECIENT_FUND\n");
	}
	else if (DECLINED_STOLEN_CARD == RetStat)
	{
		printf("Actual Result : DECLINED_STOLEN_CARD\n");
	}
	else if (APPROVED == RetStat)
	{
		printf("Actual Result : APPROVED\n");
	}

	getchar();
	/****************************** case 2 ***************************************/
	printf("\ncase 2 : LOW Balance\n");

	printf("Card Primary Account Number : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);

	printf("Trans Amount : ");
	scanf("%f", &amount);
	terminalData.transAmount = amount;
	strcpy(terminalData.transactionDate, "22/11/2022");


	transactionData.cardHolderData = cardData;
	transactionData.terminalData = terminalData;

	RetStat = recieveTransactionData(&transactionData);

	printf("Expected Result : DECLINED_INSUFFECIENT_FUND\n");

	if (FRAUD_CARD == RetStat)
	{
		printf("Actual Result : FRAUD_CARD\n");
	}
	else if (DECLINED_INSUFFECIENT_FUND == RetStat)
	{
		printf("Actual Result : DECLINED_INSUFFECIENT_FUND\n");
	}
	else if (DECLINED_STOLEN_CARD == RetStat)
	{
		printf("Actual Result : DECLINED_STOLEN_CARD\n");
	}
	else if (APPROVED == RetStat)
	{
		printf("Actual Result : APPROVED\n");
	}

	getchar();
	/****************************** case 3 ***************************************/
	printf("\ncase 3 : Blocked Account\n");

	printf("Card Primary Account Number : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);

	printf("Trans Amount : ");
	scanf("%f", &amount);
	terminalData.transAmount = amount;
	strcpy(terminalData.transactionDate, "22/11/2022");


	transactionData.cardHolderData = cardData;
	transactionData.terminalData = terminalData;

	RetStat = recieveTransactionData(&transactionData);

	printf("Expected Result : DECLINED_STOLEN_CARD\n");

	if (FRAUD_CARD == RetStat)
	{
		printf("Actual Result : FRAUD_CARD\n");
	}
	else if (DECLINED_INSUFFECIENT_FUND == RetStat)
	{
		printf("Actual Result : DECLINED_INSUFFECIENT_FUND\n");
	}
	else if (DECLINED_STOLEN_CARD == RetStat)
	{
		printf("Actual Result : DECLINED_STOLEN_CARD\n");
	}
	else if (APPROVED == RetStat)
	{
		printf("Actual Result : APPROVED\n");
	}

	getchar();
	/****************************** case 4 ***************************************/
	printf("\ncase 4 : Happy case\n");

	printf("Card Primary Account Number : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);

	printf("Trans Amount : ");
	scanf("%f", &amount);
	terminalData.transAmount = amount;
	strcpy(terminalData.transactionDate, "22/11/2022");


	transactionData.cardHolderData = cardData;
	transactionData.terminalData = terminalData;

	RetStat = recieveTransactionData(&transactionData);

	printf("Expected Result : APPROVED\n");

	if (FRAUD_CARD == RetStat)
	{
		printf("Actual Result : FRAUD_CARD\n");
	}
	else if (DECLINED_INSUFFECIENT_FUND == RetStat)
	{
		printf("Actual Result : DECLINED_INSUFFECIENT_FUND\n");
	}
	else if (DECLINED_STOLEN_CARD == RetStat)
	{
		printf("Actual Result : DECLINED_STOLEN_CARD\n");
	}
	else if (APPROVED == RetStat)
	{
		printf("Actual Result : APPROVED\n");
	}

	printf("==================================================\n");

	fflush(stdin);
	getchar();


}


void saveTransactionTest(void)
{
	ST_cardData_t cardData;
	ST_terminalData_t terminalData;
	ST_transaction transactionData;
	EN_transStat_t RetStat;

	uint8_t buffer[30];
	float amount;

	transactionData.transState = APPROVED;

	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: saveTransaction\n");

	printf("\ncase 1 : Save To File\n");

	printf("Card Holder Name : ");
	gets(buffer);
	strcpy(cardData.cardHolderName, buffer);
	printf("Card Primary Account Number : ");
	gets(buffer);
	strcpy(cardData.primaryAccountNumber, buffer);
	printf("Card Expiry Date : ");
	gets(buffer);
	strcpy(cardData.cardExpirationDate, buffer);

	printf("Trans Amount : ");
	scanf("%f", &amount);
	terminalData.transAmount = amount;
	printf("Max Amount : ");
	scanf("%f", &amount);
	getchar();
	terminalData.maxTransAmount = amount;
	printf("Transaction Date : ");
	gets(buffer);
	strcpy(terminalData.transactionDate, buffer);


	transactionData.cardHolderData = cardData;
	transactionData.terminalData = terminalData;

	RetStat = saveTransaction(&transactionData);

	printf("Expected Result : SERVER_OK\n");

	if (SAVING_FAILED == RetStat)
	{
		printf("Actual Result : SAVING_FAILED\n");
	}
	else if (SERVER_OK == RetStat)
	{
		printf("Actual Result : SERVER_OK\n");
	}

	printf("\n=================================================\n\n");
}

void listSavedTransactionsTest(void)
{
	printf("Tester Name: Hossam Elzhar\n");
	printf("Function Name: listSavedTransactions\n\n");

	listSavedTransactions();
}