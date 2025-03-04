// MainWindow.xaml
<Window x:Class="WpfApp3.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:WpfApp3"
        mc:Ignorable="d"
        Title="MainWindow" Height="450" Width="800">
    <Grid>
        <Grid.RowDefinitions>
            <RowDefinition Height="Auto"/>
            <RowDefinition Height="*"/>
            <RowDefinition Height="Auto"/>
        </Grid.RowDefinitions>

        <StackPanel Orientation="Horizontal" Margin="5">
            <Button Content="Добавить фигуру" Click="AddShapeButton_Click" Margin="5"/>
            <Button Content="Очистить" Click="ClearCanvas" Margin="5"/>
            <Button Content="Сохранить" Click="SaveDrawing" Margin="5"/>

            <Button Background="Black" Width="20" Height="20" Click="ChangeColor" Margin="5"/>
            <Button Background="Red" Width="20" Height="20" Click="ChangeColor" Margin="5"/>
            <Button Background="Green" Width="20" Height="20" Click="ChangeColor" Margin="5"/>
            <Button Background="Blue" Width="20" Height="20" Click="ChangeColor" Margin="5"/>

            <Label Content="Толщина:" Margin="5"/>
            <Slider Width="100" Minimum="1" Maximum="20" Value="5" ValueChanged="ThicknessChanged" Margin="5"/>
        </StackPanel>

        <Canvas x:Name="DrawingCanvas" Grid.Row="1" Background="White" MouseDown="Canvas_MouseDown" MouseMove="Canvas_MouseMove" MouseUp="Canvas_MouseUp"/>

        <StackPanel Grid.Row="2" Orientation="Horizontal" Margin="5">
            <ListBox x:Name="GalleryListBox" Width="200" Height="100" SelectionChanged="GalleryListBox_SelectionChanged" Margin="5"/>
            <StackPanel Orientation="Vertical" Margin="5">
                <TextBox x:Name="FolderNameTextBox" Width="150" Margin="5" Text="Новая папка"/>
                <Button Content="Создать папку" Click="CreateFolder" Margin="5"/>
                <Button Content="Переместить в папку" Click="MoveDrawing" Margin="5"/>
                <Button Content="Удалить" Click="DeleteDrawing" Margin="5"/>
            </StackPanel>
        </StackPanel>
    </Grid>
</Window>
