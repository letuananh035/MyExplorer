# Project Explorer
**MSSV : 1512003**

**Tên: Lê Tuấn Anh**

**Email: letuananh035@gmail.com**



### Các chức năng làm được:
**Upadate**
  - Cải tiến cho phép khi thay đổi kích thước treeview thì listview thay đổi theo.
  - Bổ  sung statusbar khi click vào một tập tin trong listview thì hiển thị kích thước tập tin, muilti select
  - Lưu lại kích thước cửa sổ màn hình chính và nạp lại khi chương trình chạy lên.
  
**Bản cũ**
  - TreeView hiển thị danh sách thư mục và ổ đĩa.
  - Expanding, nhấp đôi, nhấp đơn vào TreeView.
  - ListView hiển thị danh sách tập tin, thư mục và ổ đĩa.
  - Hiển thị toàn bộ thư mục và tập tin tương ứng với một đường dẫn.
  - Bấm đôi vào một thư mục sẽ thấy toàn bộ thư mục con và tập tin.
  - Hiển thị Tên, Loại, Thời gian, Dung lượng.
  - Hiển thị với icon.
  - Bấm đôi để mở tập tin.
  - Lọc tập tin hoặc thư mục trong cùng 1 thư mục cha.
  - Đường dẫn thư mục, có thể chỉnh sửa đến đường dẫn khác.
  - Phím trờ về phía trên(<-) và thư mục trở về(\.\.).
### Luồng sự kiện chính
  -  Chạy chương trình lên, hiển thị node DeskTop->This PC trên TreeView bên trái ở trạng thái collapse (thu gọn). Bấm vào sẽ xổ xuống các node con là danh sách ổ đĩa.
  - Nhấn vào node trong TreeView sẽ hiển thị các node bên ListView.
  - Nhấn vào node trong ListView để mở thư mục hoặc tập tin.
  - Tìm kiếm, đổi đường dẫn, trở về.
### Luồng sự kiện phụ
  - Bắt lỗi khi người dùng nhập sai đường dẫn.
  - Các thư mục hệ thống sẽ không thể load node con.
### Link:
  - https://github.com/letuananh035/MyExplorer.git
### Youtube:
  - https://youtu.be/j1j5v6KXHAE
