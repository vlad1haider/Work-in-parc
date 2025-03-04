MainWindows.xaml.cs
using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Windows.Threading;

namespace WpfApp3
{
    public partial class MainWindow : Window
    {
        private bool isDrawing = false; // Флаг, указывающий, рисует ли пользователь
        private Polyline currentLine; // Текущая линия, которую рисует пользователь
        private SolidColorBrush currentColor = Brushes.Black; // Цвет кисти
        private double currentThickness = 5; // Толщина линии
        private List<ImageWindow> openImageWindows = new List<ImageWindow>(); // Список открытых окон изображений

        public MainWindow()
        {
            InitializeComponent();
            LoadGallery(); // Загружаем галерею при инициализации
        }

        private void Canvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            isDrawing = true; // Начинаем рисование
            currentLine = new Polyline
            {
                Stroke = currentColor, // Устанавливаем цвет
                StrokeThickness = currentThickness, // Устанавливаем толщину
                Points = new PointCollection { e.GetPosition(DrawingCanvas) } // Добавляем начальную точку
            };
            DrawingCanvas.Children.Add(currentLine); // Добавляем линию на Canvas
        }

        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDrawing)
            {
                currentLine.Points.Add(e.GetPosition(DrawingCanvas)); // Добавляем точки к линии
            }
        }

        private void Canvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            isDrawing = false; // Завершаем рисование
        }

        private void ChangeColor(object sender, RoutedEventArgs e)
        {
            currentColor = ((Button)sender).Background as SolidColorBrush; // Меняем цвет кисти
        }

        private void ColorPicker_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            if (e.NewValue.HasValue)
            {
                currentColor = new SolidColorBrush(e.NewValue.Value); // Устанавливаем цвет из ColorPicker
            }
        }

        private void ThicknessChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            currentThickness = e.NewValue; // Меняем толщину линии
        }

        private void ClearCanvas(object sender, RoutedEventArgs e)
        {
            DrawingCanvas.Children.Clear(); // Очищаем Canvas
        }

        private void SaveDrawing(object sender, RoutedEventArgs e)
        {
            RenderTargetBitmap renderBitmap = new RenderTargetBitmap((int)DrawingCanvas.ActualWidth, (int)DrawingCanvas.ActualHeight, 96d, 96d, PixelFormats.Pbgra32);
            renderBitmap.Render(DrawingCanvas); // Рендерим Canvas в изображение

            // Путь к папке для сохранения изображений
            string folderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery");
            if (!Directory.Exists(folderPath))
            {
                Directory.CreateDirectory(folderPath); // Создаем папку, если она не существует
            }

            // Имя файла с текущей датой и временем
            string fileName = $"Рисунок_{DateTime.Now:yyyyMMdd_HHmmss}.png";
            string filePath = System.IO.Path.Combine(folderPath, fileName); // Полный путь к файлу

            using (FileStream fileStream = new FileStream(filePath, FileMode.Create))
            {
                PngBitmapEncoder encoder = new PngBitmapEncoder();
                encoder.Frames.Add(BitmapFrame.Create(renderBitmap));
                encoder.Save(fileStream); // Сохраняем изображение
            }

            LoadGallery(); // Обновляем галерею
        }

        private void LoadGallery()
        {
            GalleryListBox.Items.Clear(); // Очищаем список галереи
            string folderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery"); // Путь к папке галереи
            if (Directory.Exists(folderPath))
            {
                var files = Directory.GetFiles(folderPath, "*.png"); // Получаем все PNG файлы
                foreach (var file in files)
                {
                    Image image = new Image
                    {
                        Source = new BitmapImage(new Uri(file)), // Загружаем изображение
                        Width = 100,
                        Height = 100,
                        Margin = new Thickness(5)
                    };
                    // Создаем объект для хранения информации о рисунке
                    var galleryItem = new GalleryItem
                    {
                        Image = image,
                        FilePath = file
                    };
                    GalleryListBox.Items.Add(galleryItem); // Добавляем в список
                }
            }
        }

        private void GalleryListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (GalleryListBox.SelectedItem is GalleryItem selectedItem)
            {
                var filePath = selectedItem.FilePath; // Получаем путь к выбранному изображению

                // Загрузка изображения в память
                byte[] imageData = File.ReadAllBytes(filePath);

                var imageWindow = new ImageWindow(imageData); // Открываем новое окно с изображением
                openImageWindows.Add(imageWindow); // Добавляем окно в список открытых
                imageWindow.Closed += (s, args) => openImageWindows.Remove(imageWindow); // Удаляем из списка при закрытии
                imageWindow.Show();

                // Закрываем окно через 5 секунд (5000 миллисекунд)
                DispatcherTimer timer = new DispatcherTimer();
                timer.Interval = TimeSpan.FromSeconds(5);
                timer.Tick += (s, args) =>
                {
                    imageWindow.Close();
                    timer.Stop(); // Останавливаем таймер
                };
                timer.Start();
            }
        }

        private void DeleteDrawing(object sender, RoutedEventArgs e)
        {
            if (GalleryListBox.SelectedItem is GalleryItem selectedItem)
            {
                var filePath = selectedItem.FilePath; // Получаем путь к изображению
                MessageBoxResult result = MessageBox.Show("Вы уверены, что хотите удалить этот рисунок?", "Подтверждение", MessageBoxButton.YesNo);
                if (result == MessageBoxResult.Yes)
                {
                    try
                    {
                        // Закрываем все открытые окна с изображениями
                        foreach (var window in openImageWindows)
                        {
                            window.Close();
                        }

                        File.Delete(filePath); // Удаляем файл
                        LoadGallery(); // Обновляем галерею
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show($"Ошибка при удалении файла: {ex.Message}", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
            }
            else
            {
                MessageBox.Show("Пожалуйста, выберите рисунок для удаления.", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void CreateFolder(object sender, RoutedEventArgs e)
        {
            string folderName = FolderNameTextBox.Text; // Получаем имя новой папки
            if (!string.IsNullOrWhiteSpace(folderName))
            {
                string folderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery", folderName); // Путь к новой папке
                if (!Directory.Exists(folderPath))
                {
                    Directory.CreateDirectory(folderPath); // Создаем папку
                }
            }
        }

        private void MoveDrawing(object sender, RoutedEventArgs e)
        {
            if (GalleryListBox.SelectedItem is GalleryItem selectedItem)
            {
                var filePath = selectedItem.FilePath; // Получаем путь к изображению
                string folderName = FolderNameTextBox.Text; // Получаем имя папки для перемещения
                if (!string.IsNullOrWhiteSpace(folderName))
                {
                    string newFolderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery", folderName); // Путь к новой папке
                    if (Directory.Exists(newFolderPath))
                    {
                        string newFilePath = System.IO.Path.Combine(newFolderPath, System.IO.Path.GetFileName(filePath)); // Новый путь к файлу
                        File.Move(filePath, newFilePath); // Перемещаем файл
                        LoadGallery(); // Обновляем галерею
                    }
                }
            }
        }
    }

    // Класс для хранения информации о рисунках
    public class GalleryItem
    {
        public Image Image { get; set; }
        public string FilePath { get; set; }
    }
}

MainWindows.xaml
<Window x:Class="WpfApp3.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:toolkit="http://schemas.xceed.com/wpf/xaml/toolkit"
        xmlns:local="clr-namespace:WpfApp3"
        mc:Ignorable="d"
        Title="MainWindow" Height="550" Width="800">
    <Grid>
        <Canvas Name="DrawingCanvas" Background="White" MouseDown="Canvas_MouseDown" MouseMove="Canvas_MouseMove" MouseUp="Canvas_MouseUp" Margin="0,126,0,0"/>
        <StackPanel Orientation="Horizontal" Margin="0,48,0,408">
            <TextBox Name="FolderNameTextBox" Width="150" Margin="2" ToolTip="Имя новой папки" Height="30"/>
            <Button Content="Создать папку" Click="CreateFolder" Width="100" Margin="2" ToolTip="Создать новую папку" 
             Style="{StaticResource RoundedButtonStyle}" Height="33"/>
            <Button Content="Переместить" Click="MoveDrawing" Width="80" Margin="2" ToolTip="Переместить рисунок"
             Style="{StaticResource RoundedButtonStyle}" Height="33"/>
            <Button Content="Удалить" Click="DeleteDrawing" Width="80" Margin="2" ToolTip="Удалить рисунок" 
             Style="{StaticResource RoundedButtonStyle}" Height="33"/>
        </StackPanel>
        <StackPanel Orientation="Vertical" VerticalAlignment="Top" Height="46">
            <StackPanel Orientation="Horizontal" HorizontalAlignment="Left" Margin="0,0,0,5">
                <Button Background="Black" Click="ChangeColor" Width="30" Height="30" Margin="2" ToolTip="Черный" 
                        Style="{StaticResource RoundedButtonStyle}"/>
                <Button Background="Red" Click="ChangeColor" Width="30" Height="30" Margin="2" ToolTip="Красный" 
                        Style="{StaticResource RoundedButtonStyle}"/>
                <Button Background="Blue" Click="ChangeColor" Width="30" Height="30" Margin="2" ToolTip="Синий" 
                        Style="{StaticResource RoundedButtonStyle}"/>
                <toolkit:ColorPicker Name="ColorPicker" SelectedColorChanged="ColorPicker_SelectedColorChanged" Width="90" Margin="5" ToolTip="Выбор цвета"/>
                <Slider Minimum="1" Maximum="20" Value="5" ValueChanged="ThicknessChanged" Width="250" Margin="10" ToolTip="Толщина линии"/>
                <Button Content="Сохранить" Click="SaveDrawing" Width="80" Margin="2" ToolTip="Сохранить рисунок" 
                        Style="{StaticResource RoundedButtonStyle}"/>
                <Button Content="Очистить" Click="ClearCanvas" Width="80" Margin="2" ToolTip="Очистить холст" 
                        Style="{StaticResource RoundedButtonStyle}"/>
            </StackPanel>
        </StackPanel>
        <ListBox Name="GalleryListBox" SelectionChanged="GalleryListBox_SelectionChanged" Margin="426,46,0,408"/>
    </Grid>
</Window>

ImageWindow.xaml
<Window x:Class="WpfApp3.ImageWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:WpfApp3"
        mc:Ignorable="d"
        Title="ImageWindow" Height="450" Width="800">
    <Grid>
        <Image Name="DisplayedImage" Stretch="Uniform"/>
    </Grid>
</Window>


ImageWindow.xaml.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace WpfApp3
{

    public partial class ImageWindow : Window
    {
        public ImageWindow(byte[] imageData)
        {
            InitializeComponent();

            // Создаем изображение из массива байтов
            BitmapImage bitmap = new BitmapImage();
            using (MemoryStream memoryStream = new MemoryStream(imageData))
            {
                bitmap.BeginInit();
                bitmap.StreamSource = memoryStream;
                bitmap.CacheOption = BitmapCacheOption.OnLoad; // Используем OnLoad, чтобы избежать блокировки файла
                bitmap.EndInit();
                bitmap.Freeze(); // Замораживаем изображение для использования в UI
            }

            // Устанавливаем изображение в Image элемент в XAML
            DisplayedImage.Source = bitmap; // Предполагается, что у вас есть элемент Image с именем MyImageControl
        }
    }
}
