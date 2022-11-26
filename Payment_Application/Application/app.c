#include "app.h"

ST_cardData_t cardData;
EN_cardError_t cardReturn;
ST_terminalData_t terminalData;
EN_terminalError_t terminalReturn;
ST_transaction transactionData;
EN_serverError_t serverReturn;


void appStart(void)
{
	
	getAccountsDB();
	

	printf("\n\n======================================================================\n\n");

	cardReturn = getCardHolderName(&cardData);
	while (cardReturn == WRONG_NAME)
	{
		printf("\nYou Entered Wrong Name.\n");
		cardReturn = getCardHolderName(&cardData);
	}

	cardReturn = getCardExpiryDate(&cardData);
	while (cardReturn == WRONG_EXP_DATE)
	{
		printf("\nYou Entered Wrong EXP Date.\n");
		cardReturn = getCardExpiryDate(&cardData);
	}


	cardReturn = getCardPAN(&cardData);
	while (cardReturn == WRONG_PAN)
	{
		printf("\nYou Entered Wrong PAN Number.\n");
		cardReturn = getCardPAN(&cardData);
	}

	printf("\n================================================\n\n");


	terminalReturn = getTransactionDate(&terminalData);


	terminalReturn = isCardExpired(&cardData, &terminalData);
	if (terminalReturn == EXPIRED_CARD)
	{
		printf("Your Card Expired !\n");
		return;
	}


	terminalReturn = setMaxAmount(&terminalData, 50000.0);
	while (terminalReturn == INVALID_AMOUNT)
	{
		printf("\nInvalid Amount.\n");
		terminalReturn = setMaxAmount(&terminalData, 50000.0);
	}


	terminalReturn = getTransactionAmount(&terminalData);
	while (terminalReturn == INVALID_AMOUNT)
	{
		printf("\nInvalid Amount.\n");
		terminalReturn = getTransactionAmount(&terminalData);
	}


	terminalReturn = isBelowMaxAmount(&terminalData);
	if (terminalReturn == EXCEED_MAX_AMOUNT)
	{
		printf("\n\tDeclined \nExceed Max Balance Aount\n");
		return;
	}

	printf("\n\n======================================================================\n");
	transactionData.cardHolderData = cardData;
	transactionData.terminalData = terminalData;

	serverReturn = recieveTransactionData(&transactionData);

	if (serverReturn == FRAUD_CARD)
	{
		printf("\n\tDECLINED\nACCOUNT DOES NOT EXIST\n");
	}
	else if (serverReturn == DECLINED_INSUFFECIENT_FUND)
	{
		printf("\n\tDECLINED\nINSUFFECIENT FUND\n");
	}
	else if (serverReturn == DECLINED_STOLEN_CARD)
	{
		printf("\n\tDECLINED\nSTOLEN CARD\n");
	}
	else
	{
		printf("\n\tTRANSACTION APPROVED\n");
	}

	printf("\n================================================\n\n");
	serverReturn = saveTransaction(&transactionData);
	printf("\n================================================\n\n");
	if (serverReturn == SAVING_FAILED)
	{
		transactionData.transState = INTERNAL_SERVER_ERROR;
	}
	else
	{
		listSavedTransactions();
		printf("Updating Data Base Accounts ...\n");
		updateAccountsDB();
		
	}

	printf("\n================================================\n");
	printf("================================================\n");
}