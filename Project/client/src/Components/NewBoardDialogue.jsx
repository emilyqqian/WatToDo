import { useState } from 'react'
import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Stack,
  TextField,
} from '@mui/material'

export default function TaskboardDialog({ open, onClose, onSave }) {
  const [boardName, setBoardName] = useState('')

  const handleClose = () => {
    setBoardName('')
    onClose()
  }

  const handleSave = () => {
    if (!boardName.trim()) return
    onSave({ name: boardName.trim()})
    handleClose()
  }

  return (
    <Dialog open={open} onClose={handleClose} fullWidth maxWidth="sm">
      <DialogTitle>New Task Board</DialogTitle>
      <DialogContent>
        <Stack spacing={3} sx={{ pt: 1 }}>
          <TextField
            label="Board Name"
            value={boardName}
            onChange={(event) => setBoardName(event.target.value)}
            fullWidth
          />
        </Stack>
      </DialogContent>
      <DialogActions sx={{ px: 3, pb: 2 }}>
        <Button onClick={handleClose}>Cancel</Button>
        <Button variant="contained" onClick={handleSave}>
          Save Board
        </Button>
      </DialogActions>
    </Dialog>
  )
}