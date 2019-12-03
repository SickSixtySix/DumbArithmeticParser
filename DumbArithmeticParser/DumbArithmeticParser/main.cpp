// I'm too fucking lazy to rewrite this code to use safe functions
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

using namespace std;

// Тип данных для строкового представления лексемы
typedef char TokenValue[80];

// Перечисление типов лексем
enum TokenType
{
	TokenIntegerConst,			// Целое число
	TokenFixedPointConst,		// Число с фиксированной точкой
	TokenFloatingPointConst,	// Число с плавающей точкой
	TokenCross,					// Знак суммы
	TokenDash,					// Знак разности
	TokenStar,					// Знак произведения
	TokenSlash,					// Знак деления
	TokenOpenBrace,				// Открывающая скобка
	TokenCloseBrace,			// Закрывающая скобка
	TokenBegin,					// Специальная константа для лексемы-начала
	TokenEnd,					// Специальная константа для лексемы-конца
	TokenUnknown				// Специальная константа для неопознанной лексемы
};

// Класс лексемы
// Инкапсулирует вычисленный тип лексемы, а также её исходное строковое значение
// Содержит различные методы для работы с лексемой
class Token
{
public:

	// Переопределённый конструктор по умолчанию
	Token();

	// Создаёт лексему определённого типа (требуется для введения специальных лексем)
	Token(TokenType tokenType);

	// Выполняет чтение лексемы из стандартного потока ввода
	// Возвращает false, если достигнут конец ввода
	bool ReadToken();

	// Возвращает true если лексема является операндом
	bool IsOperand() const;

	// Возвращает true если лексема является оператором
	bool IsOperator() const;

	// Возвращает true если лексема является оператором изменения знака
	bool IsModifier() const;

	// Возвращает true если лексема является открывающей скобкой
	bool IsOpenBrace() const;

	// Возвращает true если лексема является закрывающей скобкой
	bool IsCloseBrace() const;

	// Возвращает true если лексема является началом выражения
	bool IsBegin() const;

	// Возвращает true если лексема является концом выражения
	bool IsEnd() const;

	// Возвращает true если лексема является неопознанной
	bool IsUnknown() const;

	// Возвращает тип данной лексемы
	const TokenType& GetTokenType() const;

	// Возвращает строковое значение данной лексемы
	const TokenValue& GetTokenValue() const;

private:

	// Выполняет чтение лексемы как числа
	void ReadTokenNumber();

	TokenType tokenType;	// Тип лексемы
	TokenValue tokenValue;	// Строковое значение лексемы
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

	// Пытаемся считать целую часть
	if (scanf("%[0-9]", integer))
	{
		strcat(tokenValue, integer);
		tokenType = TokenIntegerConst;
	}

	// Пытаемся считать точку
	if (scanf("%1[.]", point))
	{
		strcat(tokenValue, point);
		tokenType = TokenFixedPointConst;

		// Пытаемся считать дробную часть
		if (scanf("%[0-9]", fraction))
		{
			strcat(tokenValue, fraction);
		}
	}

	// Пытаемся считать экспоненту
	if (scanf("%1[eE]", exponent))
	{
		strcat(tokenValue, exponent);
		tokenType = TokenFloatingPointConst;

		// Пытаемся считать знак порядка
		if (scanf("%1[-+]", sign))
		{
			strcat(tokenValue, sign);
		}

		// Пытаемся считать порядок
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
	// Пропускаем пробельные символы
	scanf("%*[ \t]");

	// Определяем, завершился ли ввод
	if (scanf("%1[\n]", tokenValue))
	{
		return false;
	}

	tokenType = TokenUnknown;

	// Пытаемся считать лексему-символ
	if (scanf("%1[+]", tokenValue)) tokenType = TokenCross;
	else if (scanf("%1[-]", tokenValue)) tokenType = TokenDash;
	else if (scanf("%1[*]", tokenValue)) tokenType = TokenStar;
	else if (scanf("%1[/]", tokenValue)) tokenType = TokenSlash;
	else if (scanf("%1[(]", tokenValue)) tokenType = TokenOpenBrace;
	else if (scanf("%1[)]", tokenValue)) tokenType = TokenCloseBrace;

	// Пытаемся считать лексему-число
	if (tokenType == TokenUnknown)
	{
		ReadTokenNumber();

		// Считываем неизвестную лексему
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

// Класс выражения
// Инкапсулирует поток лексем
// Содержит методы для получения потока лексем и его анализа
class Expression
{
public:

	// Переопределённый конструктор по умолчанию
	Expression();

	// Преобразует поток символов в поток лексем (т.е. выполняет чтение выражения)
	// lexicalMistakes - ссылка на переменную для количества лексических ошибок
	void ReadExpression(int& lexicalMistakes);

	// Анализирует поток лексем с учётом синтаксиса
	// syntaxMistakes - ссылка на переменную для количества синтаксических ошибок
	void AnalyzeExpression(int& syntaxMistakes);

private:

	// Анализирует лексему-операнд
	// pToken - константная ссылка на предыдущую лексему
	// cToken - константная ссылка на текущую лексему
	// Возвращает true в случае синтаксической ошибки
	bool AnalyzeOperand(const Token& pToken, const Token& cToken);

	// Анализирует лексему-оператор
	// pToken - константная ссылка на предыдущую лексему
	// cToken - константная ссылка на текущую лексему
	// nToken - константная ссылка на следующую лексему
	// Возвращает true в случае синтаксической ошибки
	bool AnalyzeOperator(const Token& pToken, const Token& cToken, const Token& nToken);

	// Анализирует лексему открывающей скобки
	// pToken - константная ссылка на предыдущую лексему
	// cToken - константная ссылка на текущую лексему
	// Возвращает true в случае синтаксической ошибки
	bool AnalyzeOpenBrace(const Token& pToken, const Token& cToken);

	// Анализирует лексему закрывающей скобки
	// pToken - константная ссылка на предыдущую лексему
	// cToken - константная ссылка на текущую лексему
	// Возвращает true в случае синтаксической ошибки
	bool AnalyzeCloseBrace(const Token& pToken, const Token& cToken);

	// Анализирует лексему-конец
	// pToken - константная ссылка на предыдущую лексему
	// cToken - константная ссылка на текущую лексему
	// Возвращает true в случае синтаксической ошибки
	bool AnalyzeEnd(const Token& pToken, const Token& cToken);

	Token tokenList[256];	// Массив лексем (максимальное количество - 256)
	int tokenCount;			// Текущее количество лексем
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

	// Вводим специальную лексему-начало для облегчения синтаксического анализа
	tokenList[tokenCount++] = Token(TokenBegin);

	// Считываем лексемы пока не прекратится ввод символов
	while (currentToken.ReadToken())
	{
		tokenList[tokenCount++] = currentToken;

		if (!currentToken.IsUnknown())
		{
			//cout << "[ИНФО]: " << currentToken.GetTokenValue() << " => " << currentToken.GetTokenType() << endl;	
		}
		else
		{
			cout << "[ОШИБКА]: Неизвестная лексема \"" << currentToken.GetTokenValue() << "\"" << endl;
			lexicalMistakes++;
		}
	}

	// Вводим специальную лексему-конец для облегчения синтаксического анализа
	tokenList[tokenCount++] = Token(TokenEnd);
}

bool Expression::AnalyzeOperand(const Token& pToken, const Token& cToken)
{
	if (pToken.IsOperand() || pToken.IsUnknown() || pToken.IsCloseBrace())
	{
		cout << "[ОШИБКА]: Пропущен оператор или открывающая скобка перед константой \"" << cToken.GetTokenValue() << "\"" << endl;
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
				cout << "[ОШИБКА]: Пропущен операнд или открывающая скобка после знака \"" << cToken.GetTokenValue() << "\"" << endl;
				return true;
			}
		}
		else
		{
			cout << "[ОШИБКА]: Пропущен левосторонний операнд у оператора \"" << cToken.GetTokenValue() << "\"" << endl;
			return true;
		}
	}

	return false;
}

bool Expression::AnalyzeOpenBrace(const Token& pToken, const Token& cToken)
{
	if (pToken.IsOperand() || pToken.IsUnknown() || pToken.IsCloseBrace())
	{
		cout << "[ОШИБКА]: Пропущен оператор перед открывающей скобкой" << endl;
		return true;
	}

	return false;
}

bool Expression::AnalyzeCloseBrace(const Token& pToken, const Token& cToken)
{
	if (pToken.IsBegin())
	{
		cout << "[ОШИБКА]: Пропущено выражение перед закрывающей скобкой" << endl;
		return true;
	}
	else if (pToken.IsOperator())
	{
		cout << "[ОШИБКА]: Пропущен правосторонний операнд у оператора \"" << pToken.GetTokenValue() << "\"" << endl;
		return true;
	}
	else if (pToken.IsOpenBrace())
	{
		cout << "[ОШИБКА]: Пропущено выражение внутри пустых скобок" << endl;
		return true;
	}

	return false;
}

bool Expression::AnalyzeEnd(const Token& pToken, const Token& cToken)
{
	if (pToken.IsBegin())
	{
		cout << "[ОШИБКА]: Пустое выражение" << endl;
		return true;
	}
	else if (pToken.IsOperator())
	{
		cout << "[ОШИБКА]: Пропущен правосторонний операнд у оператора \"" << pToken.GetTokenValue() << endl;
		return true;
	}
	else if (pToken.IsOpenBrace())
	{
		cout << "[ОШИБКА]: Пропущено выражение после открывающей скобки" << endl;
		return true;
	}

	return false;
}

void Expression::AnalyzeExpression(int& syntaxMistakes)
{
	syntaxMistakes = 0;

	// Счётчики лишних скобок
	int extraOpenBraceCount = 0;
	int extraCloseBraceCount = 0;

	// Проверяем каждую лексему опираясь на информацию о соседних лексемах
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
		cout << "[ОШИБКА]: Не хватает " << extraOpenBraceCount << " закрывающих скобок" << endl;
		syntaxMistakes++;
	}

	if (extraCloseBraceCount > 0)
	{
		cout << "[ОШИБКА]: Не хватает " << extraCloseBraceCount << " открывающих скобок" << endl;
		syntaxMistakes++;
	}
}

int main()
{
	setlocale(0, "");

	int lexicalMistakes, syntaxMistakes;

	Expression expression;

	// Читаем выражение
	expression.ReadExpression(lexicalMistakes);

	// Анализируем выражение
	expression.AnalyzeExpression(syntaxMistakes);

	// Выводим сводку
	if (lexicalMistakes == 0 && syntaxMistakes == 0)
	{
		cout << "[ИНФО]: Выражение верно с точки зрения языка C++" << endl;
	}
	else
	{
		cout << "Лексических ошибок: " << lexicalMistakes << endl;
		cout << "Синтаксических ошибок: " << syntaxMistakes << endl;
	}

	cout << endl << endl;

	system("pause");

	return 0;
}