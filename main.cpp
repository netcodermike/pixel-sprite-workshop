#include <QtWidgets>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSaveFile>
#include <QTemporaryDir>
#include <stdexcept>
#include <functional>
QImage blank(){QImage image(16,16,QImage::Format_ARGB32);image.fill(Qt::transparent);return image;}
QByteArray encode(const QVector<QImage>&frames){QJsonArray all;for(const auto&f:frames){QJsonArray pixels;for(int y=0;y<16;y++)for(int x=0;x<16;x++)pixels.append(QString::number(f.pixel(x,y),16).rightJustified(8,'0'));all.append(pixels);}return QJsonDocument(QJsonObject{{"version",1},{"frames",all}}).toJson();}
QVector<QImage>decode(const QByteArray&bytes){
 QJsonParseError error;auto doc=QJsonDocument::fromJson(bytes,&error);if(error.error!=QJsonParseError::NoError||!doc.isObject()||doc.object()["version"].toInt()!=1||!doc.object()["frames"].isArray())throw std::runtime_error("Invalid version 1 sprite project");
 auto frames=doc.object()["frames"].toArray();if(frames.isEmpty()||frames.size()>64)throw std::runtime_error("Use 1–64 frames");QVector<QImage>out;
 for(auto value:frames){if(!value.isArray())throw std::runtime_error("Invalid frame");auto pixels=value.toArray();if(pixels.size()!=256)throw std::runtime_error("Each frame must contain 256 pixels");auto image=blank();for(int i=0;i<256;i++){QString text=pixels[i].toString();bool ok=false;quint32 p=text.toUInt(&ok,16);if(text.size()!=8||!ok)throw std::runtime_error("Invalid ARGB pixel");image.setPixel(i%16,i/16,p);}out.append(image);}return out;
}
QImage sheet(const QVector<QImage>&frames){QImage result(16*frames.size(),16,QImage::Format_ARGB32);result.fill(Qt::transparent);QPainter p(&result);for(int i=0;i<frames.size();i++)p.drawImage(i*16,0,frames[i]);return result;}
class Canvas:public QWidget{
public:
 QVector<QImage>frames{blank()};int current=0;QColor ink=QColor("#36a9a1");bool playing=false;std::function<void()>changed;
 Canvas(){setMinimumSize(450,450);setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);}
 void paintEvent(QPaintEvent*)override{
  QPainter p(this);double step=qMin(width(),height())/16.0;
  for(int y=0;y<16;y++)for(int x=0;x<16;x++){QRectF cell(x*step,y*step,step,step);p.fillRect(cell,(x+y)%2?QColor("#d7dce2"):Qt::white);p.fillRect(cell,QColor::fromRgba(frames[current].pixel(x,y)));p.setPen(QColor("#9da8b4"));p.drawRect(cell);}
 }
 void paintAt(QMouseEvent*e){if(playing)return;double step=qMin(width(),height())/16.0;int x=int(e->position().x()/step),y=int(e->position().y()/step);if(x<0||y<0||x>=16||y>=16)return;frames[current].setPixelColor(x,y,e->buttons()&Qt::RightButton?QColor(Qt::transparent):ink);update();if(changed)changed();}
 void mousePressEvent(QMouseEvent*e)override{if(e->button()==Qt::LeftButton||e->button()==Qt::RightButton)paintAt(e);}
 void mouseMoveEvent(QMouseEvent*e)override{if(e->buttons()&(Qt::LeftButton|Qt::RightButton))paintAt(e);}
};
int main(int argc,char**argv){
 if(argc==2&&QString(argv[1])=="--self-test"){
  QCoreApplication app(argc,argv);auto a=blank();a.setPixelColor(2,3,QColor("#ff0044"));QVector<QImage>frames{a,a};auto copy=decode(encode(frames));if(copy.size()!=2||copy[0].pixel(2,3)!=a.pixel(2,3))return 1;auto image=sheet(copy);if(image.width()!=32||image.height()!=16||image.pixel(18,3)!=a.pixel(2,3))return 1;QTemporaryDir dir;if(!image.save(dir.path()+"/sheet.png"))return 1;try{decode("{}");return 1;}catch(const std::exception&){}return 0;
 }
 QApplication app(argc,argv);QWidget window;window.setWindowTitle("Pixel Sprite Workshop");window.resize(850,660);auto layout=new QVBoxLayout(&window);layout->addWidget(new QLabel("16 × 16 sprite studio · left drag paints · right drag erases"));
 auto canvas=new Canvas;auto controls=new QHBoxLayout;auto frame=new QSpinBox;frame->setRange(1,1);auto status=new QLabel("Unsaved project");bool dirty=false;
 auto button=[&](const QString&name,std::function<void()>fn){auto b=new QPushButton(name);controls->addWidget(b);QObject::connect(b,&QPushButton::clicked,fn);};
 auto sync=[&](){frame->setMaximum(canvas->frames.size());frame->setValue(canvas->current+1);canvas->update();};
 canvas->changed=[&](){dirty=true;status->setText("Unsaved changes");};
 auto palette=new QHBoxLayout;for(QString color:{"#18283b","#ffffff","#36a9a1","#f8c55b","#e9768b","#7161bc","#64b5f6","#7bb66d"}){auto b=new QPushButton(color);b->setStyleSheet("background:"+color+";padding:8px");palette->addWidget(b);QObject::connect(b,&QPushButton::clicked,[=](){canvas->ink=QColor(color);});}layout->addLayout(palette);
 button("Color…",[&](){auto color=QColorDialog::getColor(canvas->ink,&window);if(color.isValid())canvas->ink=color;});controls->addWidget(new QLabel("Frame"));controls->addWidget(frame);
 button("Duplicate",[&](){if(canvas->frames.size()==64)return;canvas->frames.insert(canvas->current+1,canvas->frames[canvas->current].copy());canvas->current++;canvas->changed();sync();});
 button("Delete",[&](){if(canvas->frames.size()==1)return;canvas->frames.removeAt(canvas->current);canvas->current=qMin(canvas->current,static_cast<int>(canvas->frames.size())-1);canvas->changed();sync();});
 QTimer timer;timer.setInterval(150);QObject::connect(&timer,&QTimer::timeout,[&](){canvas->current=(canvas->current+1)%canvas->frames.size();sync();});
 button("Play / stop",[&](){canvas->playing=!canvas->playing;if(canvas->playing)timer.start();else timer.stop();});
 layout->addLayout(controls);layout->addWidget(canvas);auto files=new QHBoxLayout;layout->addLayout(files);
 auto fileButton=[&](const QString&name,std::function<void()>fn){auto b=new QPushButton(name);files->addWidget(b);QObject::connect(b,&QPushButton::clicked,fn);};
 fileButton("Save project",[&](){QString path=QFileDialog::getSaveFileName(&window,"Save sprite project",{},"Sprite JSON (*.json)");if(path.isEmpty())return;QSaveFile f(path);auto bytes=encode(canvas->frames);if(!f.open(QIODevice::WriteOnly)||f.write(bytes)!=bytes.size()||!f.commit()){QMessageBox::warning(&window,"Save failed",f.errorString());return;}dirty=false;status->setText("Project saved");});
 fileButton("Open project",[&](){if(dirty&&QMessageBox::question(&window,"Unsaved project","Discard unsaved changes?")!=QMessageBox::Yes)return;auto path=QFileDialog::getOpenFileName(&window,"Open sprite project",{},"Sprite JSON (*.json)");if(path.isEmpty())return;try{QFile f(path);if(!f.open(QIODevice::ReadOnly)||f.size()>1024*1024)throw std::runtime_error("Unreadable or oversized project");auto loaded=decode(f.readAll());timer.stop();canvas->playing=false;canvas->frames=loaded;canvas->current=0;dirty=false;sync();status->setText("Project loaded");}catch(const std::exception&e){QMessageBox::warning(&window,"Load failed",e.what());}});
 fileButton("Export PNG sheet",[&](){auto path=QFileDialog::getSaveFileName(&window,"Export horizontal sprite sheet",{},"PNG (*.png)");if(!path.isEmpty()&&!sheet(canvas->frames).save(path,"PNG"))QMessageBox::warning(&window,"Export failed","Could not write PNG");});
 layout->addWidget(status);QObject::connect(frame,&QSpinBox::valueChanged,[&](int value){canvas->current=value-1;canvas->update();});window.show();return app.exec();
}
