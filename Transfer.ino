#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// Connect Arduino UNO digital pin 11 to EEPROM SCL,
// digital pin 12 to EEPROM SDA, and ground to ground.
// DUO Portable must be disengaged from EEPROM
// before Arduino UNO can read or write.

// This program interprets the following commands
// received over serial port:
// l
// Print a list of file handles and names.
// r [file handle]
// Print the contents of a file.
// w [file handle] [offset] [text]
// Writes text into a file at the given offset.
// Note that text may not be longer than 50 characters
// because of the small serial buffer size.

#define EEPROM_DATA_PIN_OUTPUT   DDRB |= (1 << DDB4)
#define EEPROM_DATA_PIN_INPUT   DDRB &= ~(1 << DDB4)
#define EEPROM_DATA_PIN_HIGH   PORTB |= (1 << PORTB4)
#define EEPROM_DATA_PIN_LOW   PORTB &= ~(1 << PORTB4)
#define READ_EEPROM_DATA_PIN   (PINB & (1 << PINB4))

#define EEPROM_CLOCK_PIN_OUTPUT   DDRB |= (1 << DDB3)
#define EEPROM_CLOCK_PIN_INPUT   DDRB &= ~(1 << DDB3)
#define EEPROM_CLOCK_PIN_HIGH   PORTB |= (1 << PORTB3)
#define EEPROM_CLOCK_PIN_LOW   PORTB &= ~(1 << PORTB3)

#define FILE_ENTRY_SIZE 24

long findWhiteSpace(byte *buffer)
{
	long output = 0;
	while (output < 255)
	{
		byte tempCharacter = *buffer;
		if (tempCharacter == 32 || tempCharacter == 10 || tempCharacter == 0)
		{
			break;
		}
		buffer += 1;
		output += 1;
	}
	return output;
}

long getTextLength(byte *buffer)
{
	long output = 0;
	while (output < 512)
	{
		byte tempData = *buffer;
		if (tempData == 0)
		{
			break;
		}
		buffer += 1;
		output += 1;
	}
	return output;
}

long getLineLength(byte *buffer)
{
	long output = 0;
	while (output < 512)
	{
		byte tempData = *buffer;
		if (tempData == 0 || tempData == 10)
		{
			break;
		}
		buffer += 1;
		output += 1;
	}
	return output;
}

void copyText(byte *destination, byte *source)
{
	long count = 0;
	while (count < 1000)
	{
		byte tempValue = *source;
		*destination = tempValue;
		if (tempValue == 0)
		{
			break;
		}
		destination += 1;
		source += 1;
		count += 1;
	}
}

void copyData(long amount, byte *destination, byte *source)
{
	while (amount > 0)
	{
		byte tempValue = *source;
		*destination = tempValue;
		destination += 1;
		source += 1;
		amount -= 1;
	}
}

byte equalText(byte *buffer1, byte *buffer2)
{
	long index = 0;
	while (index < 512)
	{
		byte tempValue1 = *buffer1;
		byte tempValue2 = *buffer2;
		if (tempValue1 != tempValue2)
		{
			return false;
		}
		if (tempValue1 == 0)
		{
			break;
		}
		buffer1 += 1;
		buffer2 += 1;
		index += 1;
	}
	return true;
}

byte equalData(long amount, byte *buffer1, byte *buffer2)
{
	while (amount > 0)
	{
		byte tempValue1 = *buffer1;
		byte tempValue2 = *buffer2;
		if (tempValue1 != tempValue2)
		{
			return false;
		}
		buffer1 += 1;
		buffer2 += 1;
		amount -= 1;
	}
	return true;
}

void sendStartConditionToEeprom()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_DATA_PIN_LOW;
	_delay_us(6);
}

void sendStopConditionToEeprom()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_LOW;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_DATA_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	// These extra clock cycles are necessary, but
	// I'm not sure why.
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

void sendByteToEeprom(byte data)
{
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	EEPROM_DATA_PIN_OUTPUT;
	byte mask = 0x80;
	byte count = 0;
	while (count < 8)
	{
		_delay_us(6);
		byte tempResult = data & mask;
		if (tempResult > 0)
		{
			EEPROM_DATA_PIN_HIGH;
		} else {
			EEPROM_DATA_PIN_LOW;
		}
		_delay_us(6);
		EEPROM_CLOCK_PIN_HIGH;
		_delay_us(6);
		mask >>= 1;
		count += 1;
		EEPROM_CLOCK_PIN_LOW;
	}
	EEPROM_DATA_PIN_LOW;
	EEPROM_DATA_PIN_INPUT;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

byte receiveByteFromEeprom()
{
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	EEPROM_DATA_PIN_INPUT;
	byte output = 0;
	byte mask = 0x80;
	byte count = 0;
	while (count < 8)
	{
		EEPROM_CLOCK_PIN_HIGH;
		_delay_us(6);
		byte tempData = READ_EEPROM_DATA_PIN;
		if (tempData > 0)
		{
			output |= mask;
		}
		_delay_us(6);
		EEPROM_CLOCK_PIN_LOW;
		_delay_us(6);
		mask >>= 1;
		count += 1;
	}
	return output;
}

void sendAcknowledgeBit()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_LOW;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

void sendNoAcknowledgeBit()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_DATA_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_HIGH;
	_delay_us(6);
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
}

void sendReadAddressToEeprom(long address)
{
	sendStartConditionToEeprom();
	sendByteToEeprom(0xA0);
	sendByteToEeprom((address & 0x0000FF00) >> 8);
	sendByteToEeprom((address & 0x000000FF));
	sendStartConditionToEeprom();
	sendByteToEeprom(0xA1);
}

void readSequenceFromEeprom(long amount, byte *buffer, long address)
{
	sendReadAddressToEeprom(address);
	long count = 0;
	while (count < amount)
	{
		byte tempData = receiveByteFromEeprom();
		if (count < amount - 1)
		{
			sendAcknowledgeBit();
		} else {
			sendNoAcknowledgeBit();
		}
		*buffer = tempData;
		buffer += 1;
		count += 1;
	}
	sendStopConditionToEeprom();
	/*
	clearDisplay();
	drawText(0, 0, (byte *)"READ");
	drawLongNumber(0, 1, address);
	drawImage(0, 2, count, buffer - count, 100);
	promptKey();
	 */
}

void readLineFromEeprom(byte *buffer, long address)
{
	//clearDisplay();
	//drawLongNumber(0, 0, address);
	sendReadAddressToEeprom(address);
	byte count = 0;
	while (count < 200)
	{
		byte tempData = receiveByteFromEeprom();
		//drawCharacter(count * 6, 1, tempData);
		*buffer = tempData;
		if (tempData == 10 || tempData == 0)
		{
			sendNoAcknowledgeBit();
			break;
		}
		sendAcknowledgeBit();
		buffer += 1;
		count += 1;
	}
	sendStopConditionToEeprom();
	//promptKey();
	/*
	clearDisplay();
	drawText(0, 0, (byte *)"READ LINE");
	drawLongNumber(0, 1, address);
	drawImage(0, 2, count, buffer - count, 100);
	promptKey();
	 */
}

void writePageToEeprom(long address, byte amount, byte *buffer)
{
	sendStartConditionToEeprom();
	sendByteToEeprom(0xA0);
	sendByteToEeprom((address & 0x0000FF00) >> 8);
	sendByteToEeprom((address & 0x000000FF));
	byte count = 0;
	while (count < amount)
	{
		byte tempData = *buffer;
		sendByteToEeprom(tempData);
		buffer += 1;
		count += 1;
	}
	sendStopConditionToEeprom();
	_delay_ms(7);
}

void writeSequenceToEeprom(long address, long amount, byte *buffer)
{
	/*
	clearDisplay();
	drawText(0, 0, (byte *)"WRITE");
	drawLongNumber(0, 1, address);
	drawImage(0, 2, amount, buffer, 100);
	promptKey();
	 */
	long count = 0;
	while (count < amount)
	{
		long nextPageAddress = (address + 128) & 0xFFFFFF80;
		byte tempAmount = nextPageAddress - address;
		if (count + tempAmount > amount)
		{
			tempAmount = amount - count;
		}
		writePageToEeprom(address, tempAmount, buffer);
		address += tempAmount;
		buffer += tempAmount;
		count += tempAmount;
	}
	//readSequenceFromEeprom(amount, buffer - count, address - count);
}

void writeLineToEeprom(long address, byte *buffer)
{
	long tempAmount = getLineLength(buffer) + 1;
	writeSequenceToEeprom(address, tempAmount, buffer);
}

void setFileName(byte handle, byte *buffer)
{
	if (getTextLength(buffer) > 15)
	{
		return;
	}
	long tempAddress = (long)handle * FILE_ENTRY_SIZE;
	long tempLength = getTextLength(buffer);
	writeSequenceToEeprom(tempAddress, tempLength + 1, buffer);
}

void getFileName(byte *buffer, byte handle)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE;
	readLineFromEeprom(buffer, tempAddress);
}

void setFileAddress(byte handle, long address)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 16;
	writeSequenceToEeprom(tempAddress, 4, (byte *)(&address));
}

long getFileAddress(byte handle)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 16;
	long output;
	readSequenceFromEeprom(4, (byte *)(&output), tempAddress);
	return output;
}

void setFileSize(byte handle, long size)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 20;
	writeSequenceToEeprom(tempAddress, 4, (byte *)(&size));
}

long getFileSize(byte handle)
{
	long tempAddress = (long)handle * FILE_ENTRY_SIZE + 20;
	long output;
	readSequenceFromEeprom(4, (byte *)(&output), tempAddress);
	return output;
}

long getUnusedFileAddress(long size)
{
	long output = 4096;
	byte hasCollision = true;
	while (hasCollision)
	{
		hasCollision = false;
		byte tempHandle = 0;
		while (tempHandle < 170)
		{
			long tempSize = getFileSize(tempHandle);
			if (tempSize == 0xFFFFFFFF)
			{
				break;
			}
			if (tempSize > 0)
			{
				long tempAddress = getFileAddress(tempHandle);
				if (output + size > tempAddress && output < tempAddress + tempSize)
				{
					output = tempAddress + tempSize;
					hasCollision = true;
				}
			}
			tempHandle += 1;
		}
	}
	return output;
}

byte createFile(byte *buffer, long size)
{
	if (getTextLength(buffer) > 15)
	{
		return 255;
	}
	if (size > (long)30000 || size < 1)
	{
		return 255;
	}
	byte output = 0;
	while (output < 170)
	{
		long tempSize = getFileSize(output);
		if (tempSize == 0x00000000)
		{
			break;
		}
		if (tempSize == 0xFFFFFFFF)
		{
			setFileSize(output + 1, 0xFFFFFFFF);
			break;
		}
		output += 1;
	}
	long tempAddress = getUnusedFileAddress(size);
	setFileName(output, buffer);
	/*
	clearDisplay();
	drawText(0, 2, buffer);
	byte tempBuffer[17];
	getFileName(tempBuffer, 0);
	drawText(0, 3, tempBuffer);
	promptKey();
	 */
	setFileAddress(output, tempAddress);
	setFileSize(output, size);
	byte tempValue = 0;
	writeSequenceToEeprom(tempAddress, 1, &tempValue);
	/*
	clearDisplay();
	drawByteNumber(0, 0, output);
	byte tempBuffer[24];
	readSequenceFromEeprom(24, tempBuffer, (long)output * FILE_ENTRY_SIZE);
	drawImage(0, 2, 24, tempBuffer, 30);
	promptKey();
	 */
	return output;
}

void deleteFile(byte handle)
{
	setFileSize(handle, 0x00000000);
}

void resizeFile(byte handle, long size)
{
	if (size > (long)30000 || size < 1)
	{
		return;
	}
	long lastAddress = getFileAddress(handle);
	long lastSize = getFileSize(handle);
	long nextAddress = getUnusedFileAddress(size);
	setFileAddress(handle, nextAddress);
	setFileSize(handle, size);
	long tempSize;
	if (size < lastSize)
	{
		tempSize = size;
	} else {
		tempSize = lastSize;
	}
	byte tempBuffer[100];
	long index = 0;
	while (index < tempSize - 100)
	{
		readSequenceFromEeprom(100, tempBuffer, lastAddress + index);
		writeSequenceToEeprom(nextAddress + index, 100, tempBuffer);
		index += 100;
	}
	byte tempSize2 = tempSize - index;
	if (tempSize2 > 0)
	{
		readSequenceFromEeprom(tempSize2, tempBuffer, lastAddress + index);
		tempBuffer[tempSize2 - 1] = 0;
		writeSequenceToEeprom(nextAddress + index, tempSize2, tempBuffer);
	}
}

void disengageEeprom()
{
	EEPROM_DATA_PIN_INPUT;
	EEPROM_CLOCK_PIN_INPUT;
}

void engageEeprom()
{
	EEPROM_DATA_PIN_OUTPUT;
	EEPROM_CLOCK_PIN_OUTPUT;
	EEPROM_DATA_PIN_HIGH;
	EEPROM_CLOCK_PIN_LOW;
	_delay_us(6);
	sendStopConditionToEeprom();
}

int getSerialAvailable()
{
  int tempAmount = Serial.available();
  while(1 == 1)
  {
    delay(100);
    int tempAmount2 = Serial.available();
    if (tempAmount2 == tempAmount)
    {
      return tempAmount;
    }
    tempAmount = tempAmount2;
  }
}

int waitForSerialInput()
{
  while (1 == 1)
  {
    delay(20);
    int tempAmount = getSerialAvailable();
    if (tempAmount > 0)
    {
      return tempAmount;
    }
  }
}

void setup()
{
  disengageEeprom();
  Serial.begin(9600);
  Serial.println("Awake");
  /*
  engageEeprom();
  byte tempBuffer[4];
  long tempAddress = (long)7 * FILE_ENTRY_SIZE + 20;
  readSequenceFromEeprom(sizeof(tempBuffer), tempBuffer, tempAddress);
  byte index = 0;
  while (index < sizeof(tempBuffer))
  {
    byte tempData = tempBuffer[index];
    Serial.print((int)tempData);
    Serial.print(" ");
    index += 1;
  }
  Serial.println();
  disengageEeprom();
  */
  /*
  engageEeprom();
  byte tempBuffer[48];
  long tempAddress = (long)6 * FILE_ENTRY_SIZE;
  readSequenceFromEeprom(sizeof(tempBuffer), tempBuffer, tempAddress);
  byte index = 0;
  while (index < sizeof(tempBuffer))
  {
    byte tempData = tempBuffer[index];
    Serial.print((int)tempData);
    Serial.print(" ");
    index += 1;
  }
  Serial.println();
  disengageEeprom();
  */
}

void loop()
{
   long tempAmount = waitForSerialInput();
   byte tempBuffer[1000];
   Serial.readBytes((char *)tempBuffer, tempAmount);
   byte tempCharacter = *tempBuffer;
   if (tempCharacter != 'l' && tempCharacter != 'r' && tempCharacter != 'w')
   {
     disengageEeprom();
     return;
   }
   engageEeprom();
   if (tempCharacter == 'l')
   {
     byte tempHandle = 0;
     while (tempHandle < 9)
     {
       long tempSize = getFileSize(tempHandle);
       if (tempSize == 0xFFFFFFFF)
       {
         break;
       }
       if (tempSize > 0)
       {
         getFileName(tempBuffer, tempHandle);
         Serial.print((int)tempHandle);
         Serial.print(": ");
         Serial.println((char *)tempBuffer);
       }
       tempHandle += 1;
     }
   } else {
     byte tempHandle = atoi((char *)(tempBuffer + 2));
     long tempSize = getFileSize(tempHandle);
     if (tempSize < 1 || tempSize == 0xFFFFFFFF)
     {
       disengageEeprom();
       Serial.println("No such file");
       return;
     }
     long tempAddress = getFileAddress(tempHandle);
     if (tempCharacter == 'r')
     {
       long tempOffset = 0;
       while (tempOffset < tempSize)
       {
         long tempLength = tempSize - tempOffset;
         if (tempLength > 900)
         {
           tempLength = 900;
         }
         readSequenceFromEeprom(tempLength, tempBuffer, tempAddress + tempOffset);
         *(tempBuffer + tempLength) = 0;
         Serial.print((char *)tempBuffer);
         byte hasFoundNull = false;
         long tempIndex = 0;
         while (tempIndex < tempLength)
         {
           byte tempData = *(tempBuffer + tempIndex);
           if (tempData == 0)
           {
             hasFoundNull = true;
             break;
           }
           tempIndex += 1;
         }
         if (hasFoundNull)
         {
           break;
         }
         tempOffset += 900;
       }
       Serial.println();
     }
     if (tempCharacter == 'w')
     {
       long tempIndex = 2;
       tempIndex += findWhiteSpace(tempBuffer + tempIndex) + 1;
       long tempOffset = atol((char *)(tempBuffer + tempIndex));
       tempIndex += findWhiteSpace(tempBuffer + tempIndex) + 1;
       *(tempBuffer + tempAmount) = 0;
       long tempLength = tempAmount - tempIndex + 1;
       writeSequenceToEeprom(tempAddress + tempOffset, tempLength, tempBuffer + tempIndex);
       Serial.println("done");
     }
   }
   disengageEeprom();
}

