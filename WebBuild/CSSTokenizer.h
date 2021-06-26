
#pragma once

#include <memory>
#include <vector>
#include <string>

/// Token types recognized by the style tokenzier
enum class CSSTokenType
{
	null,                 /// No more tokens
	ident,                /// Identifier
	atkeyword,            /// @ keyword
	string,               /// String
	invalid,              /// Syntax error
	hash,                 /// # Hash
	number,               /// Number
	percentage,           /// Percentage number
	dimension,            /// Number with dimension
	uri,                  /// Url function
	unicode_range,        /// Unicode range
	cdo,                  /// HTLM comment begin <!--
	cdc,                  /// HTML comment end -->
	colon,                /// :
	semi_colon,           /// ;
	curly_brace_begin,    /// {
	curly_brace_end,      /// }
	bracket_begin,        /// (
	bracket_end,          /// )
	square_bracket_begin, /// [
	square_bracket_end,   /// ]
	whitespace,           /// Whitespace
	comment,              /// C style comment block
	function,             /// Function name
	includes,             /// ~=
	dashmatch,            /// |=
	delim                 /// Any other character
};

/// Style token
class CSSToken
{
public:
	/// Token type
	CSSTokenType type = CSSTokenType::null;

	/// Token value
	std::string value;

	/// Dimension for token value
	std::string dimension;

	/// Offset where token began
	size_t offset = 0;
};

/// \brief Tokenizer class that breaks text into tokens
class CSSTokenizer
{
public:
	/// Construct tokenizer for a text string
	static std::unique_ptr<CSSTokenizer> create(std::string text);

	virtual ~CSSTokenizer() = default;

	/// \brief Reads the next token
	virtual void read(CSSToken &out_token, bool eat_whitespace, bool eat_comments = true) = 0;

	/// \brief Reads all tokens belonging to a property value
	virtual std::vector<CSSToken> read_property_value(CSSToken &token, bool &out_important_flag) = 0;

	/// \brief Returns all tokens except start/end whitespace and comments
	static std::vector<CSSToken> tokenize(std::string text);
};
