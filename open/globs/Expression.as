package {
	public class Expression {
		// I use the following nasty trick for passing the string.
		// I pass it as a single-element array to simulate pass-by-reference,
		// so that these functions can chew up the string as they parse it.

		public static function parse(str:String):Expression {
			var ref = [str];
			var ret = parseExpr(ref);
			
			if (ref[0].length) {
				throw SyntaxError("Parse error, unparsed segment: " + ref[0]);
			}
			return ret;
		}
		
		// Here's the complete grammar:
		//  expr:      add-expr
		//  add-expr:  mul-expr(s [+-])
		//  mul-expr:  mul-term(s [*/])
		//  mul-term:  func-expr | '-' mul-term
		//  func-expr: func-name mul-expr | func-name '(' expr ')' | exp-expr
		//  exp-expr:  term '^' exp-expr | term
		
		static function parseExpr(str:Array):Expression {
			stripWhitespace(str);
			return parseAdd(str);
		}
		
		static function parseAdd(str:Array):Expression {
			stripWhitespace(str);
			var match;
			var rest = [ parseMult(str) ];
			stripWhitespace(str);
			while ((match = /^[+-]/.exec(str[0]))) {
				str[0] = str[0].substr(1);
				var snd = parseMult(str);
				if (match[0] == '+') {
					rest.push(snd);
				}
				else {
					rest.push(new UnaryExpr(function(x:Number):Number { return -x; }, snd));
				}
				stripWhitespace(str);
			}
			
			if (rest.length == 1) {
				return rest[0];
			}
			else {
				return new MultaryExpr(
						function(xs:Array):Number {
							var accum:Number = 0;
							for (var i:int = 0; i < xs.length; i++) {
								accum += xs[i];
							}
							return accum;
						}, rest);
			}
		}
		
		static function parseMult(str:Array):Expression {
			stripWhitespace(str);
			var match;
			var rest = [ parseMultTerm(str) ];
			stripWhitespace(str);
			while ((match = /^[*\/]/.exec(str[0]))) {
				str[0] = str[0].substr(1);
				var snd = parseMultTerm(str);
				if (match[0] == '*') {
					rest.push(snd);
				}
				else {
					rest.push(new UnaryExpr(function(x:Number):Number { return 1.0/x; }, snd));
				}
			}
			
			if (rest.length == 1) {
				return rest[0];
			}
			else {
				return new MultaryExpr(
						function(xs:Array):Number {
							var accum:Number = 1;
							for (var i:int = 0; i < xs.length; i++) {
								accum *= xs[i];
							}
							return accum;
						}, rest);
			}
		}
		
		static function parseMultTerm(str:Array):Expression {
			stripWhitespace(str);
			
			if (/^-/.test(str[0])) {
				str[0] = str[0].substr(1);
				var snd = parseMultTerm(str);
				return new UnaryExpr(function(x:Number):Number { return -x; }, snd);
			}
			else {
				return parseFunc(str);
			}
		}
		
		static function parseFunc(str:Array):Expression {
			stripWhitespace(str);
			var match;
			if ((match = /^(sin|cos|tan|abs|sqrt|exp|atan|ln)/.exec(str[0]))) {
				str[0] = str[0].substr(match[0].length);
				
				// as a special case, a parenthesized expression directly following a function
				// eats up the whole function argument:
				var arg = parseParens(str) || parseMult(str);

				switch (match[0]) {
					case "sin":
						return new UnaryExpr(function(x:Number):Number { return Math.sin(x); }, arg);
					case "cos": 
						return new UnaryExpr(function(x:Number):Number { return Math.cos(x); }, arg);
					case "tan":
						return new UnaryExpr(function(x:Number):Number { return Math.tan(x); }, arg);
					case "abs":
						return new UnaryExpr(function(x:Number):Number { return Math.abs(x); }, arg);
					case "sqrt":
						return new UnaryExpr(function(x:Number):Number { return Math.sqrt(x); }, arg);
					case "exp":
						return new UnaryExpr(function(x:Number):Number { return Math.exp(x); }, arg);
					case "atan":
						return new UnaryExpr(function(x:Number):Number { return Math.atan(x); }, arg);
					case "ln":
						return new UnaryExpr(function(x:Number):Number { return Math.log(x); }, arg);
					default:
						throw SyntaxError("Unknown function " + match[0]);
				}
			}
			else {
				return parseExp(str);
			}
		}
		
		static function parseExp(str:Array):Expression {
			stripWhitespace(str);
			
			var match;
			var fst = parseTerm(str);
			stripWhitespace(str);
			if ((match = /^\^/.exec(str[0]))) {
				str[0] = str[0].substr(1);
				var snd = parseExp(str);
				return new BinaryExpr(function (x:Number, y:Number):Number { return Math.pow(x,y); }, fst, snd);
			}
			else {
				return fst;
			}
		}
	
		
		static function parseParens(str:Array):Expression {
			stripWhitespace(str);
			var match;
			if ((match = /^\(/.exec(str[0]))) {
				str[0] = str[0].substr(match[0].length);
				var expr = parseExpr(str);
				stripWhitespace(str);
				if (!(/^\)/.test(str[0]))) {
					throw new SyntaxError("unmatched parentheses!");
				}
				str[0] = str[0].substr(1);
				return expr;
			}
			return null;
		}

		static function parseTerm(str:Array):Expression {
			stripWhitespace(str);
			
			var match;
			
			var subex = parseParens(str);
			if (subex) { return subex; }
			
			if ((match = /^-?\d+(\.\d+)?/.exec(str[0]))) {
				str[0] = str[0].substr(match[0].length);
				return new ConstExpr(parseFloat(match[0]));
			}
			
			if ((match = /^x/.exec(str[0]))) {
				str[0] = str[0].substr(match[0].length);
				return new VarExpr(match[0]);
			}
			
			if (/^\s*$/.test(str[0])) {
				throw new SyntaxError("Unexpected end of expression");
			}
			else {
				throw new SyntaxError("Unknwon term near " + str[0]);
			}
		}
		
		static function stripWhitespace(str:Array):void {
			str[0] = str[0].replace(/^\s*/g, "");
		}
		
		public function run(pad:Object):Number {
			trace("pure virtual function \"run\" called");
			return 0;
		}
	}
}

internal class MultaryExpr extends Expression {
	var func:Function;
	var exprs:Array;
	
	function MultaryExpr(fn:Function, inexprs:Array) {
		func = fn;
		exprs = inexprs;
	}
	
	public override function run(pad:Object):Number {
		var rans = new Array;
		for (var i:int = 0; i < exprs.length; i++) {
			rans.push(exprs[i].run(pad));
		}
		return func(rans);
	}
}

internal class BinaryExpr extends Expression {
	var func:Function;
	var fstExpr:Expression;
	var sndExpr:Expression;
	
	function BinaryExpr(fn:Function, fst:Expression, snd:Expression) {
		func = fn;
		fstExpr = fst;
		sndExpr = snd;
	}
	
	public override function run(pad:Object):Number {
		var fst = fstExpr.run(pad);
		var snd = sndExpr.run(pad);
		return func(fst, snd);
	}
}

internal class UnaryExpr extends Expression {
	var func:Function;
	var argExpr:Expression;
	
	function UnaryExpr(fn:Function, arg:Expression) {
		func = fn;
		argExpr = arg;
	}
	
	public override function run(pad:Object):Number {
		var arg = argExpr.run(pad);
		return func(arg);
	}
}

internal class ConstExpr extends Expression {
	var num:Number;
	
	function ConstExpr(n:Number) {
		num = n;
	}
	
	public override function run(pad:Object):Number {
		return num;
	}
}

internal class VarExpr extends Expression {
	var varName:String;
	
	function VarExpr(vName:String) {
		varName = vName;
	}
	
	public override function run(pad:Object):Number {
		return pad[varName];
	}
}