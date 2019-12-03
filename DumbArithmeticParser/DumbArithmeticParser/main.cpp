// I'm too fucking lazy to rewrite this code to use safe functions
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

using namespace std;

// ��� ������ ��� ���������� ������������� �������
typedef char TokenValue[80];

// ������������ ����� ������
enum TokenType
{
	TokenIntegerConst,			// ����� �����
	TokenFixedPointConst,		// ����� � ������������� ������
	TokenFloatingPointConst,	// ����� � ��������� ������
	TokenCross,					// ���� �����
	TokenDash,					// ���� ��������
	TokenStar,					// ���� ������������
	TokenSlash,					// ���� �������
	TokenOpenBrace,				// ����������� ������
	TokenCloseBrace,			// ����������� ������
	TokenBegin,					// ����������� ��������� ��� �������-������
	TokenEnd,					// ����������� ��������� ��� �������-�����
	TokenUnknown				// ����������� ��������� ��� ������������ �������
};

// ����� �������
// ������������� ����������� ��� �������, � ����� � �������� ��������� ��������
// �������� ��������� ������ ��� ������ � ��������
class Token
{
public:

	// ��������������� ����������� �� ���������
	Token();

	// ������ ������� ������������ ���� (��������� ��� �������� ����������� ������)
	Token(TokenType tokenType);

	// ��������� ������ ������� �� ������������ ������ �����
	// ���������� false, ���� ��������� ����� �����
	bool ReadToken();

	// ���������� true ���� ������� �������� ���������
	bool IsOperand() const;

	// ���������� true ���� ������� �������� ����������
	bool IsOperator() const;

	// ���������� true ���� ������� �������� ���������� ��������� �����
	bool IsModifier() const;

	// ���������� true ���� ������� �������� ����������� �������
	bool IsOpenBrace() const;

	// ���������� true ���� ������� �������� ����������� �������
	bool IsCloseBrace() const;

	// ���������� true ���� ������� �������� ������� ���������
	bool IsBegin() const;

	// ���������� true ���� ������� �������� ������ ���������
	bool IsEnd() const;

	// ���������� true ���� ������� �������� ������������
	bool IsUnknown() const;

	// ���������� ��� ������ �������
	const TokenType& GetTokenType() const;

	// ���������� ��������� �������� ������ �������
	const TokenValue& GetTokenValue() const;

private:

	// ��������� ������ ������� ��� �����
	void ReadTokenNumber();

	TokenType tokenType;	// ��� �������
	TokenValue tokenValue;	// ��������� �������� �������
};

Token::Token()
{
	this->tokenType = TokenUnknown;
}

Token::Token(TokenType tokenType)
{
	this->tokenType = tokenType;
}

void Token::ReadTokenNumber()
{
	char integer[80], fraction[80], point[2], exponent[2], sign[2];

	strcpy(tokenValue, "");

	// �������� ������� ����� �����
	if (scanf("%[0-9]", integer))
	{
		strcat(tokenValue, integer);
		tokenType = TokenIntegerConst;
	}

	// �������� ������� �����
	if (scanf("%1[.]", point))
	{
		strcat(tokenValue, point);
		tokenType = TokenFixedPointConst;

		// �������� ������� ������� �����
		if (scanf("%[0-9]", fraction))
		{
			strcat(tokenValue, fraction);
		}
	}

	// �������� ������� ����������
	if (scanf("%1[eE]", exponent))
	{
		strcat(tokenValue, exponent);
		tokenType = TokenFloatingPointConst;

		// �������� ������� ���� �������
		if (scanf("%1[-+]", sign))
		{
			strcat(tokenValue, sign);
		}

		// �������� ������� �������
		if (scanf("%[0-9]", fraction))
		{
			strcat(tokenValue, fraction);
		}
		else
		{
			tokenType = TokenUnknown;
		}
	}
}

bool Token::ReadToken()
{
	// ���������� ���������� �������
	scanf("%*[ \t]");

	// ����������, ���������� �� ����
	if (scanf("%1[\n]", tokenValue))
	{
		return false;
	}

	tokenType = TokenUnknown;

	// �������� ������� �������-������
	if (scanf("%1[+]", tokenValue)) tokenType = TokenCross;
	else if (scanf("%1[-]", tokenValue)) tokenType = TokenDash;
	else if (scanf("%1[*]", tokenValue)) tokenType = TokenStar;
	else if (scanf("%1[/]", tokenValue)) tokenType = TokenSlash;
	else if (scanf("%1[(]", tokenValue)) tokenType = TokenOpenBrace;
	else if (scanf("%1[)]", tokenValue)) tokenType = TokenCloseBrace;

	// �������� ������� �������-�����
	if (tokenType == TokenUnknown)
	{
		ReadTokenNumber();

		// ��������� ����������� �������
		if (tokenType == TokenUnknown)
		{
			scanf("%[^-+/*()0-9. \n\t]", tokenValue);
		}
	}

	return true;
}

bool Token::IsOperand() const
{
	switch (tokenType)
	{
	case TokenIntegerConst:
	case TokenFixedPointConst:
	case TokenFloatingPointConst:
		return true;
	}

	return false;
}

bool Token::IsOperator() const
{
	switch (tokenType)
	{
	case TokenCross:
	case TokenDash:
	case TokenStar:
	case TokenSlash:
		return true;
	}

	return false;
}

bool Token::IsModifier() const
{
	switch (tokenType)
	{
	case TokenCross:
	case TokenDash:
		return true;
	}

	return false;
}

bool Token::IsOpenBrace() const
{
	return tokenType == TokenOpenBrace;
}

bool Token::IsCloseBrace() const
{
	return tokenType == TokenCloseBrace;
}

bool Token::IsBegin() const
{
	return tokenType == TokenBegin;
}

bool Token::IsEnd() const
{
	return tokenType == TokenEnd;
}

bool Token::IsUnknown() const
{
	return tokenType == TokenUnknown;
}

const TokenType& Token::GetTokenType() const
{
	return tokenType;
}

const TokenValue& Token::GetTokenValue() const
{
	return tokenValue;
}

// ����� ���������
// ������������� ����� ������
// �������� ������ ��� ��������� ������ ������ � ��� �������
class Expression
{
public:

	// ��������������� ����������� �� ���������
	Expression();

	// ����������� ����� �������� � ����� ������ (�.�. ��������� ������ ���������)
	// lexicalMistakes - ������ �� ���������� ��� ���������� ����������� ������
	void ReadExpression(int& lexicalMistakes);

	// ����������� ����� ������ � ������ ����������
	// syntaxMistakes - ������ �� ���������� ��� ���������� �������������� ������
	void AnalyzeExpression(int& syntaxMistakes);

private:

	// ����������� �������-�������
	// pToken - ����������� ������ �� ���������� �������
	// cToken - ����������� ������ �� ������� �������
	// ���������� true � ������ �������������� ������
	bool AnalyzeOperand(const Token& pToken, const Token& cToken);

	// ����������� �������-��������
	// pToken - ����������� ������ �� ���������� �������
	// cToken - ����������� ������ �� ������� �������
	// nToken - ����������� ������ �� ��������� �������
	// ���������� true � ������ �������������� ������
	bool AnalyzeOperator(const Token& pToken, const Token& cToken, const Token& nToken);

	// ����������� ������� ����������� ������
	// pToken - ����������� ������ �� ���������� �������
	// cToken - ����������� ������ �� ������� �������
	// ���������� true � ������ �������������� ������
	bool AnalyzeOpenBrace(const Token& pToken, const Token& cToken);

	// ����������� ������� ����������� ������
	// pToken - ����������� ������ �� ���������� �������
	// cToken - ����������� ������ �� ������� �������
	// ���������� true � ������ �������������� ������
	bool AnalyzeCloseBrace(const Token& pToken, const Token& cToken);

	// ����������� �������-�����
	// pToken - ����������� ������ �� ���������� �������
	// cToken - ����������� ������ �� ������� �������
	// ���������� true � ������ �������������� ������
	bool AnalyzeEnd(const Token& pToken, const Token& cToken);

	Token tokenList[256];	// ������ ������ (������������ ���������� - 256)
	int tokenCount;			// ������� ���������� ������
};

Expression::Expression()
{
	tokenCount = 0;
}

void Expression::ReadExpression(int& lexicalMistakes)
{
	tokenCount = 0;
	lexicalMistakes = 0;

	Token currentToken;

	// ������ ����������� �������-������ ��� ���������� ��������������� �������
	tokenList[tokenCount++] = Token(TokenBegin);

	// ��������� ������� ���� �� ����������� ���� ��������
	while (currentToken.ReadToken())
	{
		tokenList[tokenCount++] = currentToken;

		if (!currentToken.IsUnknown())
		{
			//cout << "[����]: " << currentToken.GetTokenValue() << " => " << currentToken.GetTokenType() << endl;	
		}
		else
		{
			cout << "[������]: ����������� ������� \"" << currentToken.GetTokenValue() << "\"" << endl;
			lexicalMistakes++;
		}
	}

	// ������ ����������� �������-����� ��� ���������� ��������������� �������
	tokenList[tokenCount++] = Token(TokenEnd);
}

bool Expression::AnalyzeOperand(const Token& pToken, const Token& cToken)
{
	if (pToken.IsOperand() || pToken.IsUnknown() || pToken.IsCloseBrace())
	{
		cout << "[������]: �������� �������� ��� ����������� ������ ����� ���������� \"" << cToken.GetTokenValue() << "\"" << endl;
		return true;
	}

	return false;
}

bool Expression::AnalyzeOperator(const Token& pToken, const Token& cToken, const Token& nToken)
{
	if (pToken.IsBegin() || pToken.IsOperator() || pToken.IsOpenBrace())
	{
		if (cToken.IsModifier() && !nToken.IsEnd())
		{
			if (!nToken.IsOperand() && !nToken.IsUnknown() && !nToken.IsOpenBrace())
			{
				cout << "[������]: �������� ������� ��� ����������� ������ ����� ����� \"" << cToken.GetTokenValue() << "\"" << endl;
				return true;
			}
		}
		else
		{
			cout << "[������]: �������� ������������� ������� � ��������� \"" << cToken.GetTokenValue() << "\"" << endl;
			return true;
		}
	}

	return false;
}

bool Expression::AnalyzeOpenBrace(const Token& pToken, const Token& cToken)
{
	if (pToken.IsOperand() || pToken.IsUnknown() || pToken.IsCloseBrace())
	{
		cout << "[������]: �������� �������� ����� ����������� �������" << endl;
		return true;
	}

	return false;
}

bool Expression::AnalyzeCloseBrace(const Token& pToken, const Token& cToken)
{
	if (pToken.IsBegin())
	{
		cout << "[������]: ��������� ��������� ����� ����������� �������" << endl;
		return true;
	}
	else if (pToken.IsOperator())
	{
		cout << "[������]: �������� �������������� ������� � ��������� \"" << pToken.GetTokenValue() << "\"" << endl;
		return true;
	}
	else if (pToken.IsOpenBrace())
	{
		cout << "[������]: ��������� ��������� ������ ������ ������" << endl;
		return true;
	}

	return false;
}

bool Expression::AnalyzeEnd(const Token& pToken, const Token& cToken)
{
	if (pToken.IsBegin())
	{
		cout << "[������]: ������ ���������" << endl;
		return true;
	}
	else if (pToken.IsOperator())
	{
		cout << "[������]: �������� �������������� ������� � ��������� \"" << pToken.GetTokenValue() << endl;
		return true;
	}
	else if (pToken.IsOpenBrace())
	{
		cout << "[������]: ��������� ��������� ����� ����������� ������" << endl;
		return true;
	}

	return false;
}

void Expression::AnalyzeExpression(int& syntaxMistakes)
{
	syntaxMistakes = 0;

	// �������� ������ ������
	int extraOpenBraceCount = 0;
	int extraCloseBraceCount = 0;

	// ��������� ������ ������� �������� �� ���������� � �������� ��������
	for (int i = 1; i < tokenCount; i++)
	{
		const Token& pToken = tokenList[i - 1];
		const Token& cToken = tokenList[i];
		bool syntaxMistake = false;

		if (cToken.IsOperand() || cToken.IsUnknown())
		{
			syntaxMistake = AnalyzeOperand(pToken, cToken);
		}
		else if (cToken.IsOperator())
		{
			syntaxMistake = AnalyzeOperator(pToken, cToken, tokenList[i + 1]);
		}
		else if (cToken.IsEnd())
		{
			syntaxMistake = AnalyzeEnd(pToken, cToken);
		}
		else if (cToken.IsOpenBrace())
		{
			syntaxMistake = AnalyzeOpenBrace(pToken, cToken);
			extraOpenBraceCount++;
		}
		else if (cToken.IsCloseBrace())
		{
			syntaxMistake = AnalyzeCloseBrace(pToken, cToken);

			if (extraOpenBraceCount > 0)
			{
				extraOpenBraceCount--;
			}
			else
			{
				extraCloseBraceCount++;
			}
		}

		if (syntaxMistake)
		{
			syntaxMistakes++;
		}
	}

	if (extraOpenBraceCount > 0)
	{
		cout << "[������]: �� ������� " << extraOpenBraceCount << " ����������� ������" << endl;
		syntaxMistakes++;
	}

	if (extraCloseBraceCount > 0)
	{
		cout << "[������]: �� ������� " << extraCloseBraceCount << " ����������� ������" << endl;
		syntaxMistakes++;
	}
}

int main()
{
	setlocale(0, "");

	int lexicalMistakes, syntaxMistakes;

	Expression expression;

	// ������ ���������
	expression.ReadExpression(lexicalMistakes);

	// ����������� ���������
	expression.AnalyzeExpression(syntaxMistakes);

	// ������� ������
	if (lexicalMistakes == 0 && syntaxMistakes == 0)
	{
		cout << "[����]: ��������� ����� � ����� ������ ����� C++" << endl;
	}
	else
	{
		cout << "����������� ������: " << lexicalMistakes << endl;
		cout << "�������������� ������: " << syntaxMistakes << endl;
	}

	cout << endl << endl;

	system("pause");

	return 0;
}