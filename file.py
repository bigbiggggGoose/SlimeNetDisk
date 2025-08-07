# routes/file.py
from flask import request, jsonify, current_app
from flask_restful import Resource
import os
import logging
import jwt
import datetime
from werkzeug.utils import secure_filename

# 初始化日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

mysql = None
JWT_SECRET_KEY = "mP2$vL9nQ4@jR7wE5&hF3sA6*dG"

def generate_token(user):
    payload = {
        'user': user,
        'exp': datetime.datetime.utcnow() + datetime.timedelta(hours=24),
        'iat': datetime.datetime.utcnow()
    }
    token = jwt.encode(payload, JWT_SECRET_KEY, algorithm='HS256')
    return token

def verify_token(token):
    """
    验证JWT token
    """
    try:
        if not token:
            return None
        
        # 解码token
        payload = jwt.decode(token, JWT_SECRET_KEY, algorithms=['HS256'])
        
        # 检查token是否过期
        if 'exp' in payload:
            exp_timestamp = payload['exp']
            current_timestamp = datetime.datetime.utcnow().timestamp()
            if current_timestamp > exp_timestamp:
                return None
        
        return payload
    except jwt.ExpiredSignatureError:
        logger.warning("Token expired")
        return None
    except jwt.InvalidTokenError:
        logger.warning("Invalid token")
        return None
    except Exception as e:
        logger.error(f"Token verification error: {str(e)}")
        return None

def get_user_from_token(token):
    """
    从token中获取用户信息
    """
    payload = verify_token(token)
    if payload and 'user' in payload:
        return payload['user']
    return None

class FileCount(Resource):
    def post(self):
        data = request.json
        user = data.get('user')
        token = data.get('token')
        
        # Token验证
        if not verify_token(token):
            return  {'code': '111', 'msg': 'token invalid'}, 401
        
        # 验证用户
        if not user:
            return {'code': '111', 'msg': 'user parameter missing'}, 400
        
        cur = mysql.connection.cursor()
        cur.execute("SELECT COUNT(*) FROM file_info WHERE user_id=(SELECT id FROM user_info WHERE user_name=%s)",(user,))
        num = cur.fetchone()[0]
        cur.close()
        return {'code':'110','num':str(num)}

class FileList(Resource):
    def post(self):
        js = request.get_json()
        token = js.get('token')
        user = js.get('user')
        start = js.get('start', 0)
        count = js.get('count', 20)
        order = js.get('order', 'desc')
         # Token验证
        if not verify_token(token):
            return  {'code': '111', 'msg': 'token invalid'}, 401
        
        # 验证用户
        if not user:
            return {'code': '111', 'msg': 'user parameter missing'}, 400

        #保持原有排序逻辑
        order_sql = 'size ASC' if order == 'asc' else 'size DESC'

        conn = mysql.connection
        cursor = conn.cursor(dictionary=True)
        
        sql = (f"""
            SELECT 
                f.file_name, f.md5, f.create_time, f.share_status, 
                f.pv, f.url, f.size, f.type,
                u.user_name AS user  
            FROM file_info f
            JOIN user_info u ON f.user_id = u.id  
            WHERE u.user_name = %s
            ORDER BY {order_sql} 
            LIMIT %s, %s
        """)
        
        #查询
        cursor.execute(sql, (user, start, count))
        rows = cursor.fetchall()
        cursor.close()

        return  jsonify({'code':'100', 'files':rows})

class FastUpload(Resource):
      def post(self):
        js   = request.get_json(force=True)
        token = js.get('token')
        md5   = js.get('md5')
        name  = js.get('filename') or js.get('file_name')

        # Token验证
        if not verify_token(token):
            return  {'code':'111','msg':'token invalid'}, 401

        # 2) 参数完整性
        if not all([md5, name]):
            return  jsonify({'code': '302', 'msg': 'missing md5 or filename'}), 400

        try:
            conn = mysql.connection
            cursor = conn.cursor()
            cursor.execute("SELECT 1 FROM file_info WHERE md5=%s", (md5,))
            rows = cursor.fetchall()  # ✅ 必须 fetch 完结果
            cursor.close()

            if rows:
                return  jsonify({'code': '005', 'msg': 'file exists, skip upload'})
            else:
                return  jsonify({'code': '007', 'msg': 'file not found, please upload'})

        except Exception as e:
            return  jsonify({'code': '500', 'msg': f'error: {str(e)}'})

class FileUpload(Resource):
    def post(self):
        f = request.files.get('file')
        user = request.form.get('user')
        md5  = request.form.get('md5')
        token = request.form.get('token')

        # Token验证
        if not verify_token(token):
            return  {'code': '111', 'msg': 'token invalid'}, 401

        # 添加大小检查
        if f.content_length > 40 * 1024 * 1024:  # 40MB
            return {'code': '009', 'msg': 'File too large, max: 40MB'}, 400

        if not all([f, user, md5]):
            return {'code': '009', 'msg': 'missing parameters'}, 400

        filename = secure_filename(f.filename)
        save_path = os.path.join(current_app.config['UPLOAD_DIR'], filename)
        f.save(save_path)

        # 构造外链
        host = request.host_url.rstrip('/')
        file_url = f"{host}/uploads/{filename}"

        conn   = mysql.connection
        cursor = conn.cursor()
        ext= os.path.splitext(filename)[1].lstrip('.').lower()
        
        try:
            cursor.execute(
                "INSERT INTO file_info(user_id, file_name, md5, size, url, type) "
                "VALUES((SELECT id FROM user_info WHERE user_name=%s), %s, %s, %s, %s, %s)",
                (user, filename, md5, os.path.getsize(save_path), file_url, ext)
            )
            conn.commit()
        except Exception:
            cursor.close()
            return {'code': '009', 'msg': 'db insert failed'}, 500
        cursor.close()

        # 只返回 dict，让 Flask‑RESTful 自动转成 JSON
        return {'code': '008', 'msg': 'upload success', 'url': file_url}

class FileDelete(Resource):
    def post(self):
        
        try:
            data = request.get_json()
            user = data.get('user')
            md5 = data.get('md5')
            filename = data.get('filename')
            token = data.get('token')
            
            # Token验证
            if not verify_token(token):
                return  {'code': '111', 'msg': 'token invalid'}, 401

            if not all([user, md5, filename]):
                return  jsonify({'code': '014', 'msg': 'Missing parameters'})

            conn = mysql.connection
            cursor = conn.cursor()

            try:
                # 先获取用户ID
                cursor.execute("SELECT id FROM user_info WHERE user_name = %s", (user,))
                user_id = cursor.fetchone()

                if not user_id:
                    return  jsonify({'code': '014', 'msg': 'User not found'})

                # 删除文件记录
                cursor.execute(
                    "DELETE FROM file_info WHERE md5 = %s AND user_id = %s",
                    (md5, user_id[0])
                )

                # 同时从文件系统中删除文件
                try:
                    file_path = os.path.join('/data/uploads', filename)
                    if os.path.exists(file_path):
                        os.remove(file_path)
                except Exception as e:
                    logger.warning(f"Failed to delete physical file: {str(e)}")

                conn.commit()

                return  jsonify({
                    'code': '013',
                    'msg': 'File deleted successfully'
                })

            except Exception as e:
                conn.rollback()
                logger.error(f"Database error during deletion: {str(e)}")
                return  jsonify({
                    'code': '014',
                    'msg': 'Database operation failed'
                })

            finally:
                cursor.close()

        except Exception as e:
            logger.error(f"Error processing delete request: {str(e)}")
            return  {
                'code': '014',
                'msg': 'Internal server error'
            }

class FileShare(Resource):
    def post(self):
        
        try:
            # 获取 JSON 数据
            data = request.get_json()
            if not data:
                return  jsonify({"code": "011", "msg": "Missing JSON data"})
            
            # 提取参数
            filename = data.get("filename")
            md5 = data.get("md5")
            user = data.get("user")
            token = data.get("token")

            # Token验证
            if not verify_token(token):
                return  {"code": "111", "msg": "token invalid"}, 401

            # 验证必需参数
            if not all([filename, md5, user]):
                return  jsonify({"code": "011", "msg": "Missing required parameters"})
            
            # 获取数据库连接
            conn = mysql.connection
            cursor = conn.cursor()
            
            try:
                # 1. 检查是否已有其他用户分享了相同文件
                cursor.execute(
                    "SELECT COUNT(*) FROM file_info "
                    "WHERE md5 = %s AND user_id != (SELECT id FROM user_info WHERE user_name = %s) "
                    "AND share_status = 1",
                    (md5, user)
                )
                other_shared_count = cursor.fetchone()[0]
                
                if other_shared_count > 0:
                    return  jsonify({
                        "code": "012",
                        "msg": "This file has already been shared by another user"
                    })
                
                # 2. 更新分享状态
                cursor.execute(
                    "UPDATE file_info "
                    "SET share_status = 1, share_time = NOW() "
                    "WHERE md5 = %s AND user_id = (SELECT id FROM user_info WHERE user_name = %s)",
                    (md5, user)
                )
                
                # 检查是否更新成功
                if cursor.rowcount == 0:
                    return  jsonify({
                        "code": "011",
                        "msg": "File not found or not owned by user"
                    })
                
                # 提交事务
                conn.commit()
                
                # 返回成功响应
                return  jsonify({
                    "code": "010",
                    "msg": "File shared successfully"
                })
                
            except Exception as e:
                # 回滚事务
                conn.rollback()
                logger.error(f"Database error during file sharing: {str(e)}")
                return  jsonify({
                    "code": "011",
                    "msg": "Database operation failed"
                })
                
            finally:
                cursor.close()
                
        except Exception as e:
            logger.error(f"Error processing share request: {str(e)}")
            return  jsonify({
                "code": "011",
                "msg": "Internal server error"
            })


class ShareList(Resource):
    def post(self):
        js = request.get_json(force=True)
        token = js.get('token')

        conn = mysql.connection
        cursor = conn.cursor(dictionary=True)
        cursor.execute("""
            SELECT 
                f.file_name, u.user_name AS user, f.size, f.url, f.type, 
                f.share_status, f.pv, f.create_time 
            FROM file_info f
            JOIN user_info u ON f.user_id = u.id
            WHERE f.share_status = 1
            ORDER BY f.create_time DESC
        """)
        rows = cursor.fetchall()
        cursor.close()

        # 把 create_time 转成字符串
        files = []
        for r in rows:
            files.append({
                'file_name':    r['file_name'],
                'user':         r['user'],
                'size':         r['size'],
                'url':          r['url'],
                'type':         r['type'],
                'share_status': r['share_status'],
                'pv':           r['pv'],
                'create_time':  r['create_time'].isoformat()  # ← 转成字符串
            })

        return {
            'code': '120',
            'files': files
        }


class FileToggleShare(Resource):
    def post(self):
        js      = request.get_json(force=True)
        user    = js.get("user")
        token   = js.get("token")     # （可选）做身份校验
        filename= js.get("filename")
        md5     = js.get("md5")

         # Token验证
        if not verify_token(token):
            return  {"code": "111", "msg": "token invalid"}, 401

        if not all([user, filename, md5]):
            return {"code":"013","msg":"Invalid parameters"}, 400

        conn   = mysql.connection
        cur    = conn.cursor()

        try:
            # 查询当前分享状态
            cur.execute(
                "SELECT share_status FROM file_info "
                " WHERE user_id=(SELECT id FROM user_info WHERE user_name=%s) "
                "   AND file_name=%s AND md5=%s",
                (user, filename, md5)
            )
            row = cur.fetchone()
            if not row:
                return {"code":"011","msg":"file not found"}, 404

            current = row[0] or 0
            # 切换状态
            new_status = 0 if current==1 else 1
            cur.execute(
                "UPDATE file_info SET share_status=%s "
                " WHERE user_id=(SELECT id FROM user_info WHERE user_name=%s) "
                "   AND file_name=%s AND md5=%s",
                (new_status, user, filename, md5)
            )
            conn.commit()

            if new_status==1:
                return {"code":"010","msg":"share success"}
            else:
                return {"code":"012","msg":"unshare success"}
        except Exception as e:
            conn.rollback()
            logger.error(f"Database error in FileToggleShare: {str(e)}")
            return {"code":"011","msg":"database error"}, 500
        finally:
            cur.close() 
