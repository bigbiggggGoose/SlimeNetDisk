app = Flask(__name__)
app.config['MYSQL_HOST'] = '127.0.0.1'
app.config['MYSQL_USER'] = 'flaskuser'
app.config['MYSQL_PASSWORD'] = 'secure_pw'
app.config['MYSQL_DATABASE'] = 'clouddisk'  
app.config['UPLOAD_DIR'] = '/data/uploads'

mysql = MySQL(app)
api = Api(app) 

from routes.auth import UserRegister, UserLogin
from routes.file import FileCount, FileList, FileDeal, FastUpload, FileUpload, FileShare, FileDelete

# 将 mysql 对象传递给路由模块
import routes.auth as auth
import routes.file as file
auth.mysql = mysql
file.mysql = mysql

api.add_resource(auth.UserRegister, '/reg')          # 注册
api.add_resource(auth.UserLogin,    '/login')        # 登录
api.add_resource(file.FileCount,    '/myfiles/count')  # 文件总数
api.add_resource(file.FileList,     '/myfiles/list')   # 文件列表
api.add_resource(file.FileDeal,     '/dealfile')      # 分享/删除
api.add_resource(file.FastUpload,   '/md5')           # 秒传
api.add_resource(file.FileUpload,   '/upload')        # 上传
api.add_resource(file.FileShare, '/dealfile/share')
api.add_resource(file.FileDelete, '/dealfile/del')
api.add_resource(ShareList, '/sharelist')
api.add_resource(file.FileToggleShare, '/dealfile/toggle_share')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

