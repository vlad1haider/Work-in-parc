public string SelectedColor
{
    get { return _selectedColor; }
    set
    {
        if (IsValidColorString(value))
        {
            _selectedColor = value;
            OnPropertyChanged();
            if (SelectedFigure != null)
            {
                SelectedFigure.Color = _selectedColor;
                UpdateCanvas();
                SaveData();
            }
        }
        else
        {
            // Обработка недопустимого значения цвета
            MessageBox.Show("Недопустимый формат цвета. Используйте формат #RRGGBB или #AARRGGBB.", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Warning);
            // Можно восстановить предыдущее значение или установить значение по умолчанию
        }
    }
}
