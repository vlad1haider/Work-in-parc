sharp
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
using System.Linq;

namespace WpfApp3
{
    public partial class MainWindow : Window
    {
        private bool isDrawing = false;
        private Polyline currentLine;
        private SolidColorBrush currentColor = Brushes.Black;
        private double currentThickness = 5;
        private List openImageWindows = new List();

        private Rectangle selectedShape; // Выбранная фигура для перемещения/масштабирования
        private Point shapeStartPoint; // Начальная позиция фигуры при перемещении
        private Point mouseStartPoint; // Начальная позиция мыши при перемещении/масштабировании
        private bool isResizing = false;  // Флаг, указывающий, что идет изменение размера
        private Ellipse[] resizeHandles = new Ellipse[4]; // Маркеры изменения размера (углы)
        private const double ResizeHandleSize = 10;  // Размер маркеров изменения размера

        public MainWindow()
        {
            InitializeComponent();
            LoadGallery();
        }

        private void Canvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (selectedShape != null)
            {
                // Если кликнули на маркер изменения размера
                if (resizeHandles.Any(h => h.IsMouseOver))
                {
                    isResizing = true;
                    mouseStartPoint = e.GetPosition(DrawingCanvas);
                    DrawingCanvas.CaptureMouse(); // Захватываем мышь для Canvas
                    return; // Прерываем обработку, чтобы не перехватывать событие перемещения фигуры
                }

                // Если кликнули по фигуре для перемещения
                if (selectedShape.IsMouseOver)
                {
                    shapeStartPoint = new Point(Canvas.GetLeft(selectedShape), Canvas.GetTop(selectedShape));
                    mouseStartPoint = e.GetPosition(DrawingCanvas);
                    DrawingCanvas.CaptureMouse(); // Захватываем мышь для Canvas
                    return; // Прерываем обработку рисования
                }

                ClearSelection(); // Снимаем выделение, если кликнули вне фигуры и маркеров
            }

            // Если ни одна фигура не выбрана, начинаем рисование, как обычно
            isDrawing = true;
            currentLine = new Polyline
            {
                Stroke = currentColor,
                StrokeThickness = currentThickness,
                Points = new PointCollection { e.GetPosition(DrawingCanvas) }
            };
            DrawingCanvas.Children.Add(currentLine);
        }

        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (isResizing && selectedShape != null)
            {
                ResizeShape(e.GetPosition(DrawingCanvas));
                return;
            }

            if (DrawingCanvas.IsMouseCaptured && selectedShape != null) // Перемещение фигуры
            {
                MoveShape(e.GetPosition(DrawingCanvas));
                return;
            }

            if (isDrawing)
            {
                currentLine.Points.Add(e.GetPosition(DrawingCanvas));
            }
        }

        private void Canvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (isResizing)
            {
                isResizing = false;
                DrawingCanvas.ReleaseMouseCapture();
                return;
            }

            if (DrawingCanvas.IsMouseCaptured && selectedShape != null)
            {
                DrawingCanvas.ReleaseMouseCapture();
                return;
            }

            isDrawing = false;
        }

        private void AddShapeButton_Click(object sender, RoutedEventArgs e)
        {
            Rectangle rect = new Rectangle
            {
                Width = 100,
                Height = 75,
                Fill = Brushes.LightBlue,
                Stroke = Brushes.Black,
                StrokeThickness = 2
            };

            Canvas.SetLeft(rect, 50);
            Canvas.SetTop(rect, 50);
            DrawingCanvas.Children.Add(rect);

            SelectShape(rect); // Выбираем добавленную фигуру сразу
        }

        private void SelectShape(Rectangle shape)
        {
            ClearSelection(); // Снимаем выделение с предыдущей фигуры

            selectedShape = shape;

            // Создаем и отображаем маркеры изменения размера
            CreateResizeHandles();

            Canvas.SetZIndex(selectedShape, 1); // Поднимаем фигуру на передний план
        }

        private void ClearSelection()
        {
            selectedShape = null;

            // Удаляем маркеры изменения размера
            if (resizeHandles[0] != null)
            {
                foreach (var handle in resizeHandles)
                {
                    DrawingCanvas.Children.Remove(handle);
                }
            }
        }
        private void CreateResizeHandles()
        {
            // Создаем маркеры изменения размера
            for (int i = 0; i < 4; i++)
            {
                resizeHandles[i] = new Ellipse
                {
                    Width = ResizeHandleSize,
                    Height = ResizeHandleSize,
                    Fill = Brushes.Red,
                    Stroke = Brushes.Black,
                    StrokeThickness = 1
                };

                DrawingCanvas.Children.Add(resizeHandles[i]);
                Canvas.SetZIndex(resizeHandles[i], 2); // Маркеры поверх фигуры
            }

            UpdateResizeHandlePositions(); // Располагаем маркеры
        }

        private void UpdateResizeHandlePositions()
        {
            double left = Canvas.GetLeft(selectedShape);
            double top = Canvas.GetTop(selectedShape);
            double width = selectedShape.Width;
            double height = selectedShape.Height;

            Canvas.SetLeft(resizeHandles[0], left - ResizeHandleSize / 2);
            Canvas.SetTop(resizeHandles[0], top - ResizeHandleSize / 2);

            Canvas.SetLeft(resizeHandles[1], left + width - ResizeHandleSize / 2);
            Canvas.SetTop(resizeHandles[1], top - ResizeHandleSize / 2);

            Canvas.SetLeft(resizeHandles[2], left + width - ResizeHandleSize / 2);
            Canvas.SetTop(resizeHandles[2], top + height - ResizeHandleSize / 2);

            Canvas.SetLeft(resizeHandles[3], left - ResizeHandleSize / 2);
            Canvas.SetTop(resizeHandles[3], top + height - ResizeHandleSize / 2);
        }
        private void MoveShape(Point currentMousePosition)
        {
            double deltaX = currentMousePosition.X - mouseStartPoint.X;
            double deltaY = currentMousePosition.Y - mouseStartPoint.Y;

            Canvas.SetLeft(selectedShape, shapeStartPoint.X + deltaX);
            Canvas.SetTop(selectedShape, shapeStartPoint.Y + deltaY);

            UpdateResizeHandlePositions();  // Перемещаем маркеры вместе с фигурой
        }

        private void ResizeShape(Point currentMousePosition)
        {
            // Определяем, какой маркер используется для изменения размера
            int handleIndex = -1;
            for (int i = 0; i < 4; i++)
            {
                if (resizeHandles[i].IsMouseOver)
                {
                    handleIndex = i;
                    break;
                }
            }

            if (handleIndex == -1) return;  // Никакой маркер не выбран

            double left = Canvas.GetLeft(selectedShape);
            double top = Canvas.GetTop(selectedShape);
            double width = selectedShape.Width;
            double height = selectedShape.Height;

            // Вычисляем новые размеры и позицию на основе перемещения мыши и выбранного маркера
            switch (handleIndex)
            {
                case 0: // Верхний левый
                    width -= (currentMousePosition.X - mouseStartPoint.X);
                    height -= (currentMousePosition.Y - mouseStartPoint.Y);
                    left = currentMousePosition.X;
                    top = currentMousePosition.Y;
                    break;
                case 1: // Верхний правый
                    width = currentMousePosition.X - left;
                    height -= (currentMousePosition.Y - mouseStartPoint.Y);
                    top = currentMousePosition.Y;
                    break;
                case 2: // Нижний правый
                    width = currentMousePosition.X - left;
                    height = currentMousePosition.Y - top;
                    break;
                case 3: // Нижний левый
                    width -= (currentMousePosition.X - mouseStartPoint.X);
                    height = currentMousePosition.Y - top;
                    left = currentMousePosition.X;
                    break;
            }
            //Применяем изменения с проверками на минимальный размер
            selectedShape.Width = Math.Max(0, width);
            selectedShape.Height = Math.Max(0, height);
            Canvas.SetLeft(selectedShape, left);
            Canvas.SetTop(selectedShape, top);

            mouseStartPoint = currentMousePosition; // Обновляем для следующего перемещения
            UpdateResizeHandlePositions();
        }

        // Остальные методы (ChangeColor, ColorPicker_SelectedColorChanged, ThicknessChanged, ClearCanvas, SaveDrawing, LoadGallery, GalleryListBox_SelectionChanged, DeleteDrawing, CreateFolder, MoveDrawing) остаются примерно такими же, как и раньше.
        // Возможно, в LoadGallery нужно учитывать Rectangles, если их тоже сохранять.
        private void ChangeColor(object sender, RoutedEventArgs e)
        {
            currentColor = ((Button)sender).Background as SolidColorBrush;
        }

        private void ColorPicker_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            if (e.NewValue.HasValue)
            {
                currentColor = new SolidColorBrush(e.NewValue.Value);
            }
        }

        private void ThicknessChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            currentThickness = e.NewValue;
        }

        private void ClearCanvas(object sender, RoutedEventArgs e)
        {
            DrawingCanvas.Children.Clear();
            ClearSelection(); // Очищаем выделение при очистке холста
        }

        private async void SaveDrawing(object sender, RoutedEventArgs e)
        {
            RenderTargetBitmap renderBitmap = new RenderTargetBitmap((int)DrawingCanvas.ActualWidth, (int)DrawingCanvas.ActualHeight, 96d, 96d, PixelFormats.Pbgra32);
            // Скрываем маркеры перед рендерингом
            foreach (var handle in resizeHandles)
            {
                if(handle != null) handle.Visibility = Visibility.Collapsed;
            }
            renderBitmap.Render(DrawingCanvas);
            // Возвращаем маркеры после рендеринга
            foreach (var handle in resizeHandles)
            {
               if(handle != null) handle.Visibility = Visibility.Visible;
            }

            string folderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery");
            if (!Directory.Exists(folderPath))
            {
                Directory.CreateDirectory(folderPath);
            }

            string fileName = $"Рисунок_{DateTime.Now:yyyyMMdd_HHmmss}.png";
            string filePath = System.IO.Path.Combine(folderPath, fileName);

            using (FileStream fileStream = new FileStream(filePath, FileMode.Create))
            {
                PngBitmapEncoder encoder = new PngBitmapEncoder();
                encoder.Frames.Add(BitmapFrame.Create(renderBitmap));
                encoder.Save(fileStream);
            }
            LoadGallery();
        }
        private void LoadGallery()
        {
            GalleryListBox.Items.Clear();
            string folderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery");
            if (Directory.Exists(folderPath))
            {
                var files = Directory.GetFiles(folderPath, "*.png");
                foreach (var file in files)
                {
                    Image image = new Image
                    {
                        Source = new BitmapImage(new Uri(file)),
                        Width = 100,
                        Height = 100,
                        Margin = new Thickness(5)
                    };

                    var galleryItem = new GalleryItem
                    {
                        Image = image,
                        FilePath = file
                    };
                    GalleryListBox.Items.Add(galleryItem);
                }
            }
        }

        private void GalleryListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (GalleryListBox.SelectedItem is GalleryItem selectedItem)
            {
                var filePath = selectedItem.FilePath;
                byte[] imageData = File.ReadAllBytes(filePath);
                var imageWindow = new ImageWindow(imageData);
                openImageWindows.Add(imageWindow);
                imageWindow.Closed += (s, args) => openImageWindows.Remove(imageWindow);
                imageWindow.Show();
            }
        }

        private void DeleteDrawing(object sender, RoutedEventArgs e)
        {
            if (GalleryListBox.SelectedItem is GalleryItem selectedItem)
            {
                var filePath = selectedItem.FilePath;

                MessageBoxResult result = MessageBox.Show("Вы уверены, что хотите удалить этот рисунок?", "Подтверждение", MessageBoxButton.YesNo);
                if (result == MessageBoxResult.Yes)
                {
                    try
                    {
                        foreach (var window in openImageWindows.ToList())
                        {
                            if ((window.DisplayedImage.Source as BitmapImage)?.UriSource?.LocalPath == filePath)
                            {
                                window.Close();
                                openImageWindows.Remove(window);
                            }
                        }

                        File.Delete(filePath);
                        LoadGallery();
                    }
                    catch (UnauthorizedAccessException ex)
                    {
                        MessageBox.Show($"Нет прав на удаление файла: {ex.Message}", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Error);
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
            string folderName = FolderNameTextBox.Text;
            if (!string.IsNullOrWhiteSpace(folderName))
            {
                string folderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery", folderName);
                if (!Directory.Exists(folderPath))
                {
                    Directory.CreateDirectory(folderPath);
                }
            }
        }

        private void MoveDrawing(object sender, RoutedEventArgs e)
        {
            if (GalleryListBox.SelectedItem is GalleryItem selectedItem)
            {
                var filePath = selectedItem.FilePath;
                string folderName = FolderNameTextBox.Text;

                if (!string.IsNullOrWhiteSpace(folderName))
                {
                    string newFolderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery", folderName);
                    if (Directory.Exists(newFolderPath))
                    {
                        string newFilePath = System.IO.Path.Combine(newFolderPath, System.IO.Path.GetFileName(filePath));
                        File.Move(filePath, newFilePath);
                        LoadGallery();
                    }
                }
            }
        }
    }
    public class GalleryItem
    {
        public Image Image { get; set; }
        public string FilePath { get; set; }
    }
}
