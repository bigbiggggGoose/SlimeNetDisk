# routes/auth.py
from flask import request
from flask_restful import Resource
from werkzeug.security import generate_password_hash, check_password_hash
import mysql.connector
import jwt, datetime

mysql = None
JWT_SECRET_KEY = "mP2$vL9nQ4@jR7wE5&hF3sA6*dG"

def generate_token(user):
    payload = {
        'user': user,
        'exp': datetime.datetime.utcnow() + datetime.timedelta(hours=24),
        'iat': datetime.datetime.utcnow()
    }
    token = jwt.encode(payload, JWT_SECRET_KEY, algorithm='HS256')
    # 关键：PyJWT 2.x 返回 bytes，需要转成 str
    if isinstance(token, bytes):
        token = token.decode('utf-8')
    return token

class UserRegister(Resource):
    def post(self):
        try:
            data = request.get_json()
            username = data['userName']  # 注意变量名统一
            nickname = data['nickName']
            pwd = data['firstPwd']
            phone = data['phone']
            email = data['email']
            
            # 生成密码哈希
            hashed_pwd = generate_password_hash(pwd)
            
            conn = mysql.connection
            cursor = conn.cursor()
            
            try:
                cursor.execute(
                    "INSERT INTO user_info(user_name, nick_name, password, phone, email) "
                    "VALUES(%s, %s, %s, %s, %s)",
                    (username, nickname, hashed_pwd, phone, email)
                )
                conn.commit()
                
                return {
                    'code': '000',
                    'msg': 'register success'
                }
                
            except Exception as e:  # 捕获通用异常
                conn.rollback()
                logger.error(f"Registration error: {str(e)}")
                
                # 判断是否为重复用户错误
                if "Duplicate entry" in str(e):
                    return {
                        'code': '003',
                        'msg': 'User already exists'
                    }, 400
                
                return {
                    'code': '002',
                    'msg': 'Registration failed'
                }, 500
                
            finally:
                cursor.close()
                
        except KeyError as e:
            return {
                'code': '001',
                'msg': f'Missing required field: {str(e)}'
            }, 400
        except Exception as e:
            logger.error(f"Unexpected error: {str(e)}")
            return {
                'code': '002',
                'msg': 'Internal server error'
            }, 500

class UserLogin(Resource):
    def post(self):
        data = request.get_json()
        user = data.get('user')
        pwd  = data.get('pwd')
        cur = mysql.connection.cursor()
        cur.execute("SELECT password FROM user_info WHERE user_name=%s", (user,))
        row = cur.fetchone()
        cur.close()
        if row and check_password_hash(row[0], pwd):
            token = generate_token(user)
            return {"code": "000", "token": token}
        return {'code': '001', 'msg': 'login failed'} 
