import React, { useState } from 'react'
import './App.css'

function TaskboardDialog({ isOpen, onClose, onSave }) {
  const [boardName, setBoardName] = useState('')
  const [boardType, setBoardType] = useState('personal')

  if (!isOpen) return null

  const handleSave = () => {
    if (boardName.trim() === '') return
    onSave({ name: boardName.trim(), type: boardType })
    setBoardName('')
    setBoardType('personal')
  }

  return (
    <div className="dialog-backdrop" onClick={onClose}>
      <div className="dialog" onClick={e => e.stopPropagation()}>
        <h3>New Board</h3>
        <label>
          Board Name:
          <input
            type="text"
            value={boardName}
            onChange={e => setBoardName(e.target.value)}
          />
        </label>
        <label>
          Board Type:
          <select
            value={boardType}
            onChange={e => setBoardType(e.target.value)}
          >
            <option value="personal">Personal</option>
            <option value="shared">Shared</option>
          </select>
        </label>
        <div className="dialog-buttons">
          <button onClick={handleSave}>Save</button>
          <button onClick={onClose}>Cancel</button>
        </div>
      </div>
    </div>
  )
}

function App() {
  const [personalBoards, setPersonalBoards] = useState([
    'Personal Board 1',
    'Personal Board 2',
    'Personal Board 3',
  ])
  const [sharedBoards, setSharedBoards] = useState([
    'Shared Board A',
    'Shared Board B',
    'Shared Board C',
  ])
  const [isDialogOpen, setIsDialogOpen] = useState(false)

  const openDialog = () => setIsDialogOpen(true)
  const closeDialog = () => setIsDialogOpen(false)

  const handleSaveBoard = ({ name, type }) => {
    if (type === 'personal') {
      setPersonalBoards(prev => [...prev, name])
    } else {
      setSharedBoards(prev => [...prev, name])
    }
    closeDialog()
  }

  return (
    <>
      <h1>Wat To Do</h1>
      <button className="new-board-button" onClick={openDialog}>+ New Board</button>
      <div className="board-container">
        <h2>Personal Task Boards</h2>
        {personalBoards.map((board, i) => (
          <button key={i} className="board-button">{board}</button>
        ))}
      </div>
      <div className="board-container">
        <h2>Shared Task Boards</h2>
        {sharedBoards.map((board, i) => (
          <button key={i} className="board-button">{board}</button>
        ))}
      </div>
      <TaskboardDialog isOpen={isDialogOpen} onClose={closeDialog} onSave={handleSaveBoard} />
    </>
  )
}

export default App
