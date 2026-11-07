#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

class ASTNode {
public:
	virtual ~ASTNode() = default;
	virtual std::string toJSON() const = 0;
	virtual std::string generateCode(int indent = 0) const = 0;
	virtual int evaluate(std::map<std::string, int>& env) = 0;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

inline std::string getIndent(int indent) {
	return std::string(indent * 4, ' ');
}

class BlockNode : public ASTNode {
public:
	std::vector<ASTNodePtr> statements;
	void addStatement(ASTNodePtr stmt) { statements.push_back(stmt); }

	std::string toJSON() const override {
		std::string json = "{\"name\": \"Block\", \"children\": [";
		for (size_t i = 0; i < statements.size(); ++i) {
			json += statements[i]->toJSON();
			if (i + 1 < statements.size()) json += ", ";
		}
		json += "]}";
		return json;
	}

	std::string generateCode(int indent = 0) const override {
		std::string code;
		for (const auto& stmt : statements) {
			code += stmt->generateCode(indent);
		}
		return code;
	}
	int evaluate(std::map<std::string, int>& env) override {
            int lastVal = 0;
            for (const auto& stmt : statements) {
                lastVal = stmt->evaluate(env);
            }
            return lastVal;
        }
};

class NumberNode : public ASTNode {
public:
	int value;
	NumberNode(int val) : value(val) {}

	std::string toJSON() const override {
		return "{\"name\": \"Num: " + std::to_string(value) + "\"}";
	}

	std::string generateCode(int = 0) const override {
		return std::to_string(value);
	}
	int evaluate(std::map<std::string, int>& env) override{
	        return value;
	}
};

class VariableNode : public ASTNode {
public:
	std::string name;
	VariableNode(const std::string& n) : name(n) {}

	std::string toJSON() const override {
		return "{\"name\": \"Var: " + name + "\"}";
	}

	std::string generateCode(int = 0) const override {
		return name;
	}
	int evaluate(std::map<std::string, int>& env) override {
	        if  (env.find(name) == env.end()){
	                std::cerr << "Warning: undefined variable '" << name << "', defaulting to 0\n";
	                return 0;
	        }
	        return env[name];
	}
};

class BinaryOpNode : public ASTNode {
public:
	std::string op;
	ASTNodePtr left;
	ASTNodePtr right;

	BinaryOpNode(const std::string& o, ASTNodePtr l, ASTNodePtr r)
		: op(o), left(l), right(r) {}

	std::string toJSON() const override {
		return "{\"name\": \"Op: " + op + "\", \"children\": [" +
			left->toJSON() + ", " + right->toJSON() + "]}";
	}

	std::string generateCode(int = 0) const override {
		return "(" + left->generateCode() + " " + op + " " + right->generateCode() + ")";
	}
	int evaluate(std::map<std::string, int>& env) override {
                int l = left->evaluate(env);
                int r = right->evaluate(env);

                if (op == "+") return l + r;
                if (op == "-") return l - r;
                if (op == "*") return l * r;
                if (op == "/") return (r != 0) ? (l / r) : 0;
                if (op == ">") return l > r;
                if (op == "<") return l < r;
                if (op == ">=") return l >= r;
                if (op == "<=") return l <= r;
                if (op == "==") return l == r;
                if (op == "!=") return l != r;

                return 0;
          }
};

class AssignNode : public ASTNode {
public:
	std::string varName;
	ASTNodePtr expr;

	AssignNode(const std::string& name, ASTNodePtr e) : varName(name), expr(e) {}

	std::string toJSON() const override {
		return "{\"name\": \"Assign (=)\", \"children\": [{\"name\": \"ID: " +
			varName + "\"}, " + expr->toJSON() + "]}";
	}

	std::string generateCode(int indent = 0) const override {
		return getIndent(indent) + varName + " = " + expr->generateCode() + "\n";
	}
	int evaluate(std::map<std::string, int>& env) override {
            int val = expr->evaluate(env);
            env[varName] = val;
            return val;
        }
};

class IfNode : public ASTNode {
public:
	ASTNodePtr condition;
	ASTNodePtr thenBranch;
	ASTNodePtr elseBranch;

	IfNode(ASTNodePtr cond, ASTNodePtr thenB, ASTNodePtr elseB = nullptr)
		: condition(cond), thenBranch(thenB), elseBranch(elseB) {}

	std::string toJSON() const override {
		std::string json = "{\"name\": \"IfStatement\", \"children\": [";
		json += "{\"name\": \"Condition\", \"children\": [" + condition->toJSON() + "]}, ";
		json += "{\"name\": \"Then\", \"children\": [" + thenBranch->toJSON() + "]}";
		if (elseBranch) {
		json += ", {\"name\": \"Else\", \"children\": [" + elseBranch->toJSON() + "]}";
		}
		json += "]}";
		return json;
	}

	std::string generateCode(int indent = 0) const override {
		std::string code = getIndent(indent) + "if " + condition->generateCode() + ":\n";
		code += thenBranch->generateCode(indent + 1);
		if (elseBranch) {
			code += getIndent(indent) + "else:\n";
			code += elseBranch->generateCode(indent + 1);
	}
		return code;
	}
	int evaluate(std::map<std::string, int>& env) override {
            if (condition->evaluate(env)) {
                return thenBranch->evaluate(env);
            } else if (elseBranch) {
                return elseBranch->evaluate(env);
            }
            return 0;
        }
};

class WhileNode : public ASTNode {
public:
	ASTNodePtr condition;
	ASTNodePtr body;

	WhileNode(ASTNodePtr cond, ASTNodePtr b) : condition(cond), body(b) {}
	std::string toJSON() const override {
        	return "{\"name\": \"WhileLoop\", \"children\": [{\"name\": \"Condition\", \"children\": [" +
		condition->toJSON() + "]}, {\"name\": \"Body\", \"children\": [" +
		body->toJSON() + "]}]}";
    	}

	std::string generateCode(int indent = 0) const override {
            std::string code = getIndent(indent) + "while " + condition->generateCode() + ":\n";
            code += body->generateCode(indent + 1);
            return code;
        }
        int evaluate(std::map<std::string, int>& env) override {
        while (condition->evaluate(env)) {
            body->evaluate(env);
        }
        return 0;
    }
};
