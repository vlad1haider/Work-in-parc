# Work-in-parc

🎨 Сохранение рисунка в файл  
Используем RenderTargetBitmap:  

private void SaveDrawing(object sender, RoutedEventArgs e)
{
    string folderPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery");
    Directory.CreateDirectory(folderPath);
    
    string fileName = $"Рисунок_{DateTime.Now:yyyyMMdd_HHmmss}.png";
    string filePath = Path.Combine(folderPath, fileName);
    
    RenderTargetBitmap renderBitmap = new RenderTargetBitmap(
        (int)DrawingCanvas.ActualWidth, (int)DrawingCanvas.ActualHeight, 96d, 96d, PixelFormats.Pbgra32);
    renderBitmap.Render(DrawingCanvas);

    using (FileStream fs = new FileStream(filePath, FileMode.Create))
    {
        PngBitmapEncoder encoder = new PngBitmapEncoder();
        encoder.Frames.Add(BitmapFrame.Create(renderBitmap));
        encoder.Save(fs);
    }
    
    LoadGallery(); // Обновляем список рисунков
}

 🎨 Отображение галереи  
Загружаем файлы из Gallery и показываем их в ListBox:  

<ListBox Name="GalleryList" SelectionChanged="GalleryList_SelectionChanged">
    <ListBox.ItemTemplate>
        <DataTemplate>
            <StackPanel>
                <Image Source="{Binding}" Width="100" Height="100"/>
                <TextBlock Text="{Binding}" TextAlignment="Center"/>
            </StackPanel>
        </DataTemplate>
    </ListBox.ItemTemplate>
</ListBox>

private void LoadGallery()
{
    string folderPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery");
    if (!Directory.Exists(folderPath)) return;

    var images = Directory.GetFiles(folderPath, "*.png");
    GalleryList.ItemsSource = images;
}

 🎨 Удаление рисунка  
Добавляем кнопку и логику удаления файла:  

<Button Content="Удалить" Click="DeleteDrawing"/>

private void DeleteDrawing(object sender, RoutedEventArgs e)
{
    if (GalleryList.SelectedItem == null) return;

    string filePath = GalleryList.SelectedItem.ToString();
    if (File.Exists(filePath))
    {
        if (MessageBox.Show("Удалить этот рисунок?", "Подтверждение", MessageBoxButton.YesNo) == MessageBoxResult.Yes)
        {
            File.Delete(filePath);
            LoadGallery();
        }
    }
}

🎨 Сортировка по папкам  
Добавляем поле для выбора папки и кнопку для перемещения файла:  

<TextBox Name="FolderNameBox" Width="150" PlaceholderText="Введите имя папки"/>
<Button Content="Переместить" Click="MoveToFolder"/>

private void MoveToFolder(object sender, RoutedEventArgs e)
{
    if (GalleryList.SelectedItem == null || string.IsNullOrWhiteSpace(FolderNameBox.Text)) return;

    string folderPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Gallery", FolderNameBox.Text);
    Directory.CreateDirectory(folderPath);

    string selectedFile = GalleryList.SelectedItem.ToString();
    string fileName = Path.GetFileName(selectedFile);
    string newFilePath = Path.Combine(folderPath, fileName);

    File.Move(selectedFile, newFilePath);
    LoadGallery();
}
