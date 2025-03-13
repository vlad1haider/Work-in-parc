System.FormatException
  HResult=0x80131537
  Сообщение = Недопустимый маркер.
  Источник = PresentationCore
  Трассировка стека:
   в MS.Internal.Parsers.ParseBrush(String brush, IFormatProvider formatProvider, ITypeDescriptorContext context)
   в System.Windows.Media.Brush.Parse(String value, ITypeDescriptorContext context)
   в System.Windows.Media.BrushConverter.ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, Object value)
   в System.ComponentModel.TypeConverter.ConvertFromString(String text)
   в WpfSketchApp.MainWindow.UpdateCanvas() в C:\Users\student\source\repos\WpfApp4\WpfApp4\MainWindow.xaml.cs:строка 242
   в WpfSketchApp.MainWindow.set_CurrentSketch(Sketch value) в C:\Users\student\source\repos\WpfApp4\WpfApp4\MainWindow.xaml.cs:строка 50
   в WpfSketchApp.MainWindow..ctor() в C:\Users\student\source\repos\WpfApp4\WpfApp4\MainWindow.xaml.cs:строка 106
   в System.RuntimeType.CreateInstanceDefaultCtor(Boolean publicOnly, Boolean wrapExceptions)
