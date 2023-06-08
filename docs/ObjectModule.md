# ����ģ��

��ϵͳ�Ķ���ģ����ơ�

## ����ģ�͸���

��ϵͳ��MULE��Ϊ�������������ı���/���ı�������ơ�
���ܹ����ж����ƶ�д�Ļ�����Ԫ��������Object����������һ�����ݽṹ�Ķ������ǽṹ�塰Structure����
�ṹ���е����ݣ������ֶΡ�Field����

��ϵͳ���϶��ṹ����������ʽ�ڶ��������������֯��ÿ�������Ӧһ����Table�������к��н��մ洢��Structure����ˣ���ϵͳҪ�����е���䶼��ʽ������

## ����������

��Ϊ�϶����б��ڽṹ��ṹһ�£���ˣ���ϵͳ�涨��Tableֻ�ڿ�ʼ��ȡ֮ǰ�����ṹ�����ݷ���һ�ε����ݴ�������DataHandler����֮��ֻ���մ˽ṹ���������ݴ��������Ͷ�ȡ�������ݡ�

ͬ����д��ʱ��ֻ�ڿ�ʼд��֮ǰ�����ṹ�����ݷ���һ�ε����ݴ�������֮�󲻶ϴ����ݴ�������ȡ������д�롣

���ݴ������������¼��ķ�ʽ�������ݵĶ�ȡ��д�룬ÿ�����У��ṹ�壩��ʼ��ȡ/д���ǰ�󶼲���һ���¼���ÿ����Ԫ���ֶΣ���ȡ��д��ǰҲ�ֱ�����¼����Լ��������ݣ���

```
         (Field/Structure/...)
+--------+ Object             +--------------+ DataHandler   +-----------+
| Binary |------------------->| MultiValue   |-------------->| Text/     |
| Stream |<-------------------|              |<--------------| Lua Table |
+--------+             Object +--------------+   DataHandler +-----------+

��ȡ��������
Table::Read()+---->DataHandler::BeforeSheetRead()
             +---->Structure::Read()*����
             +---->Structure::Read()*����
			 +...
			 +---->DataHandler::AfterSheetRead()

Structure::Read()+---->DataHandler::BeforeRecordRead()
                 +---->Field::Read()+----->BinaryStream::Read()
                 |                  +----->DataHandler::OnCellRead()
                 |                  +----->Field(next)::Read()+---->BinaryStream::Read() // Field ���Ϊ��ʽ��ȡ
		         |                                            +---->DataHandler::OnCellRead()
        	     |                                            +---->Field(next)::Read()+...
		         +---->DataHandler::AfterSheetRead/Write()
```

## ��չ�Ժ��Զ���
��ϵͳ��ͨ��Ԥ����������ķ�ʽ��Ϊ�޸����¿����ԡ�

### ���ݴ�����
���ݴ������Ƕ����ݽ��д���Ĳ��֣��Ƕ����ƶ������������ת���Ľ��档

�ļ� mule/Data/Basic/DataHandler.h����Ϊ���ݴ������������Լ�����Ĳ����У�

- OnSheetReadingStarted() : void
- OnSheetWritingStarted() : void
- OnSheetReadingEnded() : void
- OnSheetWritingEnded() : void
- OnRecordReadingStarted() : void
- OnRecordWritingStarted() : void
- OnRecordReadingEnded() : void
- OnRecordWritingEnded() : void
- OnCellRead(const MultiValue &) : void
- OnCellWrite() : MultiValue

��˳����һ����/�ṹ�忪ʼ/������/дʱ���¼����������һ����Ԫ���ֶΣ���/дǰ/����¼�������ͨ����Щ�¼���׷�ٶ�д���ȡ�
���ݴ�������Ҫ��OnCellRead�д����ȡ�������ݣ�����д���ļ������׼������У�Ҳ�����ǽ���ת��Ϊ Lua ����
���Ƶأ���д��ʱ�����ݴ�������Ҫ�� Lua �����ļ������׼��������ȡ���ݣ���ȷ�ش������װ������Ҫд���ʵ�����ݷ��ظ����÷������ֶδ���������

### ����
������������ɵģ����ɶ����ݴ������������в����������Object ���Ǵ����࣬���ڶ�����ӿڡ�

�ļ� mule/Data/Basic/Object.h��

�̳в�ʵ�ִ��࣬���� mule::Data::ObjectManager ע�ἴ��ʹ�䱻��ϵͳ���֡�

#### �ֶ�
Field �Ƕ�������ࡣ�󲿷�ʱ��Ӧ�ü̳�����ʵ�ֶ���������д����Ĳ�����

�ļ� mule/Data/Basic/Field.h��

�ֶ���Ҫʵ�����·�����DoRead() �� DoWrite()��

MultiValue DoRead(FieldReadWriteContext &context) ���ڴӶ���������ȡ����ȡ���õĽ��ֱ��ͨ�� MultiValue ���ظ��������ڶ���������

int DoWrite(FieldReadWriteContext &context, const MultiValue &value) ���ڽ����� value д�����������
ϵͳԼ���˺�������ֵ����Ϊ 0�����ȡ�����жϲ��������ָ���Ĵ����룬������ֵ����
ϵͳ��������������Դ����ض���������ֻ���ж϶�д�������������룬����⽫���²��ɻָ��Ĵ�����ע����һ�㣬��������ǿ��Իָ��ģ����� Field �ڲ����д���

�ֶλ���ʵ�� size_t FieldSize() ��������������ռ�Ŀռ䡣ͨ����С��Field ���ܹ���������д�Ĵ�����һ���Խ��������ݶ��뵽�ڴ洦����Ԥ�����ܣ���δʵ�֣�

##### �ֶδ�����

�ֶ���Ҫ����ֶδ�����ʹ�á��ֶδ����������ֶδ������󲢷������úõ��ֶζ���

�ļ� mule/Data/FieldCreator.h

�ֶδ�������Ҫʵ�� Basic::Field *DoCreateField(FieldCreatingInfo &info) �������ֶεĴ����������Ӧ�Ĵ�����Ϣ������Ӧ���뷵�� nullptr ���ƽ�����������һ����������

#### ���������

����ȫ�ֹ�������ע����Ϣ���ṩ��ע��Ķ��󡣲��й�һ���ֶδ��������������Դ����ֶεĴ�������

�ļ� mule/Data/ObjectManager.h

��ϵͳ��ʼ��ʱ��Ӧ������Ҫ�Ķ���ע�ᵽ������������ʹ��Ӧ�������ϵͳ�ɼ�������ʹʵ���˶���Ҳ�޷����д���

ͨ�� ObjectManager::Register() �����ڱ�������ע�ᡣ�Ƽ���ע����ɱ��������������������ڣ���������������ʵ����ָ�롣

ʹ�� ObjectManager::Release() �����ͷŶ�����ͬʱ��ɾ������ʵ��

## ��ͼ
��ûд�꣬���ˣ�
### �����ռ� mule::Data::Basic

```mermaid
class DataHandler {
	#OnSheetReadingStarted() : void
	#OnSheetWritingStarted() : void
	#OnSheetReadingEnded() : void
	#OnSheetWritingEnded() : void
	#OnRecordReadingStarted() : void
	#OnRecordWritingStarted() : void
	#OnRecordReadingEnded() : void
	#OnRecordWritingEnded() : void
	#OnCellRead(const MultiValue &) : void
	#OnCellWrite() : MultiValue
}
```
