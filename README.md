private void UpdateCanvas()
{
    MainCanvas.Children.Clear();

    if (CurrentSketch?.Components != null)
    {
        foreach (var figure in CurrentSketch.Components)
        {
            SolidColorBrush brush = Brushes.Black; // Значение по умолчанию

            try
            {
                brush = (SolidColorBrush)new BrushConverter().ConvertFromString(figure.Color);
            }
            catch (Exception ex)
            {
                // Обработка ошибки преобразования цвета
                Console.WriteLine($"Ошибка преобразования цвета для фигуры {figure.Id}: {ex.Message}");
                //Можно установить цвет по умолчанию или пропустить фигуру.
                //brush = Brushes.Red; // Установите цвет по умолчанию
            }

            Rectangle rect = new Rectangle
            {
                Width = figure.Size.Width,
                Height = figure.Size.Height,
                Fill = brush,
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
