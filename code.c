sharp
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace WpfSketchApp
{
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        private const string DataFilePath = "sketches.json";
        private const double DefaultSquareSize = 100;
        private const double ResizeHandleSize = 10;

        private ObservableCollection<Sketch> _sketches = new ObservableCollection<Sketch>();
        private Sketch _currentSketch;
        private Figure _selectedFigure;
        private Point _startPoint;
        private bool _isDragging;
        private ResizeHandle _resizingHandle;
        private string _selectedColor = "#4287f5"; // Default color

        public ObservableCollection<Sketch> Sketches
        {
            get { return _sketches; }
            set
            {
                _sketches = value;
                OnPropertyChanged();
            }
        }

        public Sketch CurrentSketch
        {
            get { return _currentSketch; }
            set
            {
                _currentSketch = value;
                OnPropertyChanged();
                UpdateCanvas();
            }
        }

        public Figure SelectedFigure
        {
            get { return _selectedFigure; }
            set
            {
                if (_selectedFigure != value)
                {
                    if (_selectedFigure != null)
                    {
                        HideResizeHandles();
                    }

                    _selectedFigure = value;
                    OnPropertyChanged();
                    OnPropertyChanged(nameof(IsFigureSelected)); // Notify that a figure is selected or not

                    if (_selectedFigure != null)
                    {
                        ShowResizeHandles();
                    }
                }
            }
        }

        public string SelectedColor
        {
            get { return _selectedColor; }
            set
            {
                _selectedColor = value;
                OnPropertyChanged();
                if (SelectedFigure != null)
                {
                    SelectedFigure.Color = _selectedColor;
                    UpdateCanvas(); // Update the canvas to reflect the color change
                    SaveData();      // Save the data after changing the color
                }
            }
        }

        public bool IsFigureSelected => SelectedFigure != null; // Property to determine if a figure is selected

        public event PropertyChangedEventHandler PropertyChanged;

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
            LoadData();

            if (Sketches.Any())
            {
                CurrentSketch = Sketches.First();
            }
            else
            {
                CreateNewSketch();
            }
        }

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private void LoadData()
        {
            if (File.Exists(DataFilePath))
            {
                try
                {
                    string jsonString = File.ReadAllText(DataFilePath);
                    var data = JsonSerializer.Deserialize<SketchData>(jsonString);
                    if (data?.Sketches != null)
                    {
                        Sketches = new ObservableCollection<Sketch>(data.Sketches);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Ошибка при загрузке данных: {ex.Message}", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            else
            {
                Sketches = new ObservableCollection<Sketch>();
            }
        }

        private void SaveData()
        {
            try
            {
                var data = new SketchData { Sketches = Sketches.ToList() };
                string jsonString = JsonSerializer.Serialize(data, new JsonSerializerOptions { WriteIndented = true });
                File.WriteAllText(DataFilePath, jsonString);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Ошибка при сохранении данных: {ex.Message}", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void AddSquareButton_Click(object sender, RoutedEventArgs e)
        {
            if (CurrentSketch == null) return;

            var newFigure = new Figure
            {
                Id = Guid.NewGuid(),
                Type = "figure",
                Position = new Point { X = 50, Y = 50 },
                Size = new Size { Width = DefaultSquareSize, Height = DefaultSquareSize },
                Color = SelectedColor // Use the selected color
            };

            CurrentSketch.Components.Add(newFigure);
            UpdateCanvas();
        }

        private void Canvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.Source is Rectangle)
            {
                SelectedFigure = (e.Source as Rectangle).DataContext as Figure;
                _startPoint = e.GetPosition(MainCanvas);
                _isDragging = true;
                MainCanvas.CaptureMouse();
            }
            else if (e.Source is Ellipse resizeHandle)
            {
                SelectedFigure = (resizeHandle.DataContext as Figure);
                _resizingHandle = (ResizeHandle)Enum.Parse(typeof(ResizeHandle), resizeHandle.Tag.ToString());
                _startPoint = e.GetPosition(MainCanvas);
                _isDragging = true;
                MainCanvas.CaptureMouse();
            }
            else
            {
                SelectedFigure = null;
            }
        }

        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isDragging && SelectedFigure != null)
            {
                Point currentPoint = e.GetPosition(MainCanvas);
                double deltaX = currentPoint.X - _startPoint.X;
                double deltaY = currentPoint.Y - _startPoint.Y;

                if (_resizingHandle == ResizeHandle.None)
                {
                    // Движение фигуры
                    SelectedFigure.Position = new Point
                    {
                        X = SelectedFigure.Position.X + deltaX,
                        Y = SelectedFigure.Position.Y + deltaY
                    };
                    _startPoint = currentPoint;
                }
                else
                {
                    // Изменение размера фигуры
                    ResizeFigure(deltaX, deltaY);
                    _startPoint = currentPoint;
                }

                UpdateCanvas();
            }
        }

        private void Canvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            _isDragging = false;
            MainCanvas.ReleaseMouseCapture();
            _resizingHandle = ResizeHandle.None;
            SaveData();
        }

        private void UpdateCanvas()
        {
            MainCanvas.Children.Clear();

            if (CurrentSketch?.Components != null)
            {
                foreach (var figure in CurrentSketch.Components)
                {
                    Rectangle rect = new Rectangle
                    {
                        Width = figure.Size.Width,
                        Height = figure.Size.Height,
                        Fill = (SolidColorBrush)new BrushConverter().ConvertFromString(figure.Color),
                        DataContext = figure
                    };

                    rect.MouseDown += Canvas_MouseDown;
                    rect.MouseMove += Canvas_MouseMove;
                    rect.MouseUp += Canvas_MouseUp;

                    Canvas.SetLeft(rect, figure.Position.X);
                    Canvas.SetTop(rect, figure.Position.Y);

                    MainCanvas.Children.Add(rect);
                }

                if (SelectedFigure != null)
                {
                    ShowResizeHandles();
                }
            }
        }

        private void ResizeFigure(double deltaX, double deltaY)
        {
            if (SelectedFigure == null) return;

            switch (_resizingHandle)
            {
                case ResizeHandle.TopLeft:
                    SelectedFigure.Position = new Point
                    {
                        X = SelectedFigure.Position.X + deltaX,
                        Y = SelectedFigure.Position.Y + deltaY
                    };
                    SelectedFigure.Size = new Size
                    {
                        Width = Math.Max(0, SelectedFigure.Size.Width - deltaX),
                        Height = Math.Max(0, SelectedFigure.Size.Height - deltaY)
                    };
                    break;

                case ResizeHandle.TopRight:
                    SelectedFigure.Position = new Point
                    {
                        Y = SelectedFigure.Position.Y + deltaY
                    };
                    SelectedFigure.Size = new Size
                    {
                        Width = Math.Max(0, SelectedFigure.Size.Width + deltaX),
                        Height = Math.Max(0, SelectedFigure.Size.Height - deltaY)
                    };
                    break;

                case ResizeHandle.BottomLeft:
                    SelectedFigure.Position = new Point
                    {
                        X = SelectedFigure.Position.X + deltaX,
                    };
                    SelectedFigure.Size = new Size
                    {
                        Width = Math.Max(0, SelectedFigure.Size.Width - deltaX),
                        Height = Math.Max(0, SelectedFigure.Size.Height + deltaY)
                    };
                    break;

                case ResizeHandle.BottomRight:
                    SelectedFigure.Size = new Size
                    {
                        Width = Math.Max(0, SelectedFigure.Size.Width + deltaX),
                        Height = Math.Max(0, SelectedFigure.Size.Height + deltaY)
                    };
                    break;
            }
        }

        private void ShowResizeHandles()
        {
            if (SelectedFigure == null) return;

            AddResizeHandle(SelectedFigure, ResizeHandle.TopLeft);
            AddResizeHandle(SelectedFigure, ResizeHandle.TopRight);
            AddResizeHandle(SelectedFigure, ResizeHandle.BottomLeft);
            AddResizeHandle(SelectedFigure, ResizeHandle.BottomRight);
        }

        private void HideResizeHandles()
        {
            var handlesToRemove = MainCanvas.Children.OfType<Ellipse>().ToList();
            foreach (var handle in handlesToRemove)
            {
                MainCanvas.Children.Remove(handle);
            }
        }

        private void AddResizeHandle(Figure figure, ResizeHandle handleType)
        {
            Ellipse handle = new Ellipse
            {
                Width = ResizeHandleSize,
                Height = ResizeHandleSize,
                Fill = Brushes.Black,
                DataContext = figure,
                Tag = handleType.ToString()
            };

            handle.MouseDown += Canvas_MouseDown;
            handle.MouseMove += Canvas_MouseMove;
            handle.MouseUp += Canvas_MouseUp;

            double left = 0;
            double top = 0;

            switch (handleType)
            {
                case ResizeHandle.TopLeft:
                    left = figure.Position.X - ResizeHandleSize / 2;
                    top = figure.Position.Y - ResizeHandleSize / 2;
                    break;
                case ResizeHandle.TopRight:
                    left = figure.Position.X + figure.Size.Width - ResizeHandleSize / 2;
                    top = figure.Position.Y - ResizeHandleSize / 2;
                    break;
                case ResizeHandle.BottomLeft:
                    left = figure.Position.X - ResizeHandleSize / 2;
                    top = figure.Position.Y + figure.Size.Height - ResizeHandleSize / 2;
                    break;
                case ResizeHandle.BottomRight:
                    left = figure.Position.X + figure.Size.Width - ResizeHandleSize / 2;
                    top = figure.Position.Y + figure.Size.Height - ResizeHandleSize / 2;
                    break;
            }

            Canvas.SetLeft(handle, left);
            Canvas.SetTop(handle, top);

            MainCanvas.Children.Add(handle);
        }

        private void CreateNewSketch()
        {
            var newSketch = new Sketch
            {
                Name = $"Sketch {Sketches.Count + 1}",
                Components = new ObservableCollection<Figure>()
            };
            Sketches.Add(newSketch);
            CurrentSketch = newSketch;
        }

        private void NewSketchButton_Click(object sender, RoutedEventArgs e)
        {
            CreateNewSketch();
        }

        private void SketchList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (SketchList.SelectedItem is Sketch selectedSketch)
            {
                CurrentSketch = selectedSketch;
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            SaveData();
        }
    }

    public class Sketch : INotifyPropertyChanged
    {
        private string _name;
        private ObservableCollection<Figure> _components = new ObservableCollection<Figure>();

        public string Name
        {
            get { return _name; }
            set
            {
                _name = value;
                OnPropertyChanged();
            }
        }

        public ObservableCollection<Figure> Components
        {
            get { return _components; }
            set
            {
                _components = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class Figure : INotifyPropertyChanged
    {
        private Guid _id;
        private string _type;
        private Point _position;
        private Size _size;
        private string _color;

        public Guid Id
        {
            get { return _id; }
            set
            {
                _id = value;
                OnPropertyChanged();
            }
        }

        public string Type
        {
            get { return _type; }
            set
            {
                _type = value;
                OnPropertyChanged();
            }
        }

        public Point Position
        {
            get { return _position; }
            set
            {
                _position = value;
                OnPropertyChanged();
            }
        }

        public Size Size
        {
            get { return _size; }
            set
            {
                _size = value;
                OnPropertyChanged();
            }
        }

        public string Color
        {
            get { return _color; }
            set
            {
                _color = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class SketchData
    {
        public List<Sketch> Sketches { get; set; }
    }

    public enum ResizeHandle
    {
        None,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    }
}
