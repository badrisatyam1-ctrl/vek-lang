#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "token.hpp"
#include "ast.hpp"

using std::vector;
using std::string;
using std::unique_ptr;
using std::make_unique;
using std::move;

class Parser {
public:
    explicit Parser(const vector<Token>& tkns) : tokens(tkns) {}

    vector<unique_ptr<Stmt>> parse_program() {
        vector<unique_ptr<Stmt>> statements;
        while (!is_at_end()) statements.push_back(parse_statement());
        return statements;
    }

private:
    const vector<Token>& tokens;
    size_t current = 0;

    bool is_at_end() const { return current >= tokens.size(); }
    bool check(TokenType type) const { return !is_at_end() && tokens[current].type == type; }
    Token previous() const { return tokens[current - 1]; }
    bool match(TokenType type) { if (!check(type)) return false; ++current; return true; }
    Token consume(TokenType expected, const string& message) {
        if (!check(expected)) throw std::runtime_error(message);
        return tokens[current++];
    }

    unique_ptr<Stmt> parse_statement() {
        if (check(TokenType::LET)) return parse_declaration();
        if (check(TokenType::FUNC)) return parse_function();
        if (check(TokenType::IF)) return parse_if();
        
        if (check(TokenType::RET)) {
            consume(TokenType::RET, "Expected 'ret'");
            unique_ptr<Expr> value = nullptr;
            if (!check(TokenType::SEMICOLON)) value = parse_expression();
            consume(TokenType::SEMICOLON, "Expected ';'");
            return make_unique<ReturnStmt>(move(value));
        }

        if (check(TokenType::LBRACE)) return parse_block();

        if (check(TokenType::IDENTIFIER)) {
            Token name = tokens[current];
            if (name.text == "sys") {
                current++;
                consume(TokenType::COLON_COLON, "Expected '::'");
                Token func = consume(TokenType::IDENTIFIER, "Expected function name");
                if (func.text == "out") {
                    consume(TokenType::LPAREN, "Expected '('");
                    auto expr = parse_expression();
                    consume(TokenType::RPAREN, "Expected ')'");
                    consume(TokenType::SEMICOLON, "Expected ';'");
                    return make_unique<PrintStmt>(move(expr));
                }
                throw std::runtime_error("Unknown system function");
            }
        }

        auto expr = parse_expression();
        if (match(TokenType::EQUALS)) {
            auto value = parse_expression();
            consume(TokenType::SEMICOLON, "Expected ';'");
            return make_unique<AssignStmt>(move(expr), move(value));
        }
        if (match(TokenType::SEMICOLON)) {
            return make_unique<ExprStmt>(move(expr));
        }
        throw std::runtime_error("Parsing failed");
    }

    unique_ptr<Stmt> parse_block() {
        consume(TokenType::LBRACE, "Expected '{'");
        vector<unique_ptr<Stmt>> body;
        while (!check(TokenType::RBRACE) && !is_at_end()) body.push_back(parse_statement());
        consume(TokenType::RBRACE, "Expected '}'");
        return make_unique<BlockStmt>(move(body));
    }

    unique_ptr<Stmt> parse_if() {
        consume(TokenType::IF, "Expected 'if'");
        consume(TokenType::LPAREN, "Expected '('");
        auto condition = parse_expression();
        consume(TokenType::RPAREN, "Expected ')'");
        
        auto then_branch = parse_block();

        unique_ptr<Stmt> else_branch = nullptr;
        if (match(TokenType::ELSE)) {
            if (check(TokenType::IF)) {
                else_branch = parse_if();
            } else {
                else_branch = parse_block();
            }
        }

        return make_unique<IfStmt>(move(condition), move(then_branch), move(else_branch));
    }

    unique_ptr<Stmt> parse_function() {
        consume(TokenType::FUNC, "Expected 'func'");
        Token name = consume(TokenType::IDENTIFIER, "Expected function name");
        consume(TokenType::LPAREN, "Expected '('");
        
        vector<Param> params;
        if (!check(TokenType::RPAREN)) {
            do {
                TokenType p_type = parse_type();
                Token p_name = consume(TokenType::IDENTIFIER, "Expected parameter name");
                params.emplace_back(p_type, p_name.text);
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')'");
        TokenType return_type = parse_type();
        
        consume(TokenType::LBRACE, "Expected '{'");
        vector<unique_ptr<Stmt>> body;
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            body.push_back(parse_statement());
        }
        consume(TokenType::RBRACE, "Expected '}'");

        return make_unique<FuncDecl>(return_type, name.text, move(params), move(body));
    }

    TokenType parse_type() {
        if (match(TokenType::INT32)) return TokenType::INT32;
        if (match(TokenType::INT64)) return TokenType::INT64;
        if (match(TokenType::FLOAT32)) return TokenType::FLOAT32;
        if (match(TokenType::FLOAT64)) return TokenType::FLOAT64;
        if (match(TokenType::STR)) return TokenType::STR;
        if (match(TokenType::CHAR)) return TokenType::CHAR;
        if (match(TokenType::BOOL)) return TokenType::BOOL;
        if (match(TokenType::VOID)) return TokenType::VOID;
        throw std::runtime_error("Expected a type");
    }

    unique_ptr<Stmt> parse_declaration() {
        consume(TokenType::LET, "Expected 'let'");
        TokenType type = parse_type();
        Token name = consume(TokenType::IDENTIFIER, "Expected a variable name");
        
        bool is_array = false;
        unique_ptr<Expr> array_size = nullptr;
        
        if (match(TokenType::LBRACKET)) {
            is_array = true;
            if (!check(TokenType::RBRACKET)) {
                array_size = parse_expression();
            }
            consume(TokenType::RBRACKET, "Expected ']'");
        }

        unique_ptr<Expr> initializer = nullptr;
        if (match(TokenType::EQUALS)) {
            initializer = parse_expression();
        }
        
        consume(TokenType::SEMICOLON, "Expected ';'");
        return make_unique<VarDecl>(type, name.text, is_array, move(array_size), move(initializer));
    }

    unique_ptr<Expr> parse_expression() {
        auto expr = parse_term();
        while (match(TokenType::PLUS) || match(TokenType::MINUS) || match(TokenType::GREATER_THAN) || match(TokenType::LESS_THAN)) { // Added comparisons
            TokenType op = previous().type;
            auto right = parse_term();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    unique_ptr<Expr> parse_term() {
        auto expr = parse_factor();
        while (match(TokenType::STAR) || match(TokenType::SLASH)) {
            TokenType op = previous().type;
            auto right = parse_factor();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    unique_ptr<Expr> parse_factor() {
        auto expr = parse_primary();

        while (true) {
            if (match(TokenType::LBRACKET)) {
                auto index = parse_expression();
                consume(TokenType::RBRACKET, "Expected ']'");
                expr = make_unique<ArrayAccessExpr>(move(expr), move(index));
            } 
            else if (match(TokenType::DOT)) {
                Token method = consume(TokenType::IDENTIFIER, "Expected method name");
                consume(TokenType::LPAREN, "Expected '('");
                vector<unique_ptr<Expr>> args;
                if (!check(TokenType::RPAREN)) {
                    do {
                        args.push_back(parse_expression());
                    } while (match(TokenType::COMMA));
                }
                consume(TokenType::RPAREN, "Expected ')'");
                expr = make_unique<MethodCallExpr>(move(expr), method.text, move(args));
            } 
            else {
                break;
            }
        }
        return expr;
    }

    unique_ptr<Expr> parse_primary() {
        if (match(TokenType::INT64_LITERAL)) return make_unique<Int64Literal>(std::stoll(previous().text));
        if (match(TokenType::FLOAT64_LITERAL)) return make_unique<Float64Literal>(std::stod(previous().text));
        if (match(TokenType::BOOL_LITERAL)) return make_unique<BoolLiteral>(previous().text == "true");
        if (match(TokenType::STR_LITERAL)) return make_unique<StringLiteral>(previous().text);
        if (match(TokenType::CHAR_LITERAL)) return make_unique<CharLiteral>(previous().text[0]);

        if (match(TokenType::LBRACE)) {
            vector<unique_ptr<Expr>> elements;
            if (!check(TokenType::RBRACE)) {
                do {
                    elements.push_back(parse_expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACE, "Expected '}'");
            return make_unique<ArrayLiteral>(move(elements));
        }

        if (match(TokenType::IDENTIFIER)) {
            string name = previous().text;
            if (match(TokenType::LPAREN)) {
                vector<unique_ptr<Expr>> args;
                if (!check(TokenType::RPAREN)) {
                    do { args.push_back(parse_expression()); } while (match(TokenType::COMMA));
                }
                consume(TokenType::RPAREN, "Expected ')'");
                return make_unique<CallExpr>(name, move(args));
            }
            return make_unique<VariableExpr>(name);
        }

        if (match(TokenType::LPAREN)) {
            auto expr = parse_expression();
            consume(TokenType::RPAREN, "Expected ')'");
            return expr;
        }
        throw std::runtime_error("Expected a value, variable, or '('");
    }
};